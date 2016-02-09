#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/timeb.h>

#include "homp.h"

/**
 * notifying the helper threads to work on the offloading specified in off_info arg
 * It always start with copyto and may stops after copyto for target data
 * master is just the thread that will store
 */
void omp_offloading_start(omp_offloading_info_t *off_info, int free_after_completion) {
	off_info->free_after_completion = free_after_completion;
	omp_grid_topology_t * top = off_info->top;
    /* generate master trace file */

	off_info->start_time = read_timer_ms(); /* only for the first time */

	int i;
	for (i = 0; i < top->nnodes; i++) {
		omp_device_t * dev = &omp_devices[top->idmap[i]];
		if (dev->offload_request != NULL) {
			fprintf(stderr, "device %d is not ready for answering your request, offloading_start: %X. It is a bug so far\n", dev->id, off_info);
		}
		dev->offload_request = off_info;
		//printf("offloading to device: %d, %X\n", i, off_info);
		/* TODO: this is data race if multiple host threads try to offload to the same devices,
		 * FIX is to use cas operation to update this field
		 */
	}

	pthread_barrier_wait(&off_info->barrier);

	if (off_info->count) off_info->count++; /* recurring, increment the number of offloading */
	off_info->compl_time = read_timer_ms();

#if defined (OMP_BREAKDOWN_TIMING)
	pthread_barrier_wait(&off_info->barrier); /* this one make sure the profiling is collected */
#endif
}


long secondary_offload_cycle (omp_offloading_info_t * off_info, omp_offloading_t * off, omp_event_t* events, int seqid, int misc_event_index) {
	int i;
#if defined (OMP_BREAKDOWN_TIMING)
	omp_event_record_start(&events[map_dist_alloc_event_index]);
#endif
    long total;
	if (off_info->loop_redist_needed) total = omp_loop_iteration_dist(off);
	else total = 0;
	if (total == 0) return 0;
	//case OMP_OFFLOADING_MAPMEM:
	off->stage = OMP_OFFLOADING_MAPMEM;
	/* init data map and dev memory allocation */
	/***************** for each mapped variable that has to and tofrom, if it has region mapped to this __ndev_i__ id, we need code here *******************************/
	for (i = 0; i < off_info->num_mapped_vars; i++) {
		/* we handle inherited map here, by each helper thread, and we only update the off object (not off_info)*/
		omp_data_map_info_t *map_info = &off_info->data_map_info[i];

		int inherited = 1;
		omp_data_map_t *map = omp_map_offcache_iterator(off, i, &inherited);
		if (inherited) continue;
		if (map_info->remap_needed)omp_data_map_dist(map, seqid); {
			if (map->access_level != OMP_DATA_MAP_ACCESS_LEVEL_MALLOC) omp_map_malloc(map, off);
		}
		//omp_print_data_map(map);
	}

#if defined (OMP_BREAKDOWN_TIMING)
	omp_event_record_stop(&events[map_dist_alloc_event_index]);
#endif

	if (off_info->type == OMP_OFFLOADING_STANDALONE_DATA_EXCHANGE) goto data_exchange;

//	case OMP_OFFLOADING_COPYTO:
	{
		omp_offloading_copyto:;
		off->stage = OMP_OFFLOADING_COPYTO;
#if defined (OMP_BREAKDOWN_TIMING)
		if (off_info->num_mapped_vars > 0)
			omp_event_record_start(&events[acc_mapto_event_index]);
#endif
		for (i = 0; i < off->num_maps; i++) {
			int inherited;
			omp_data_map_t *map = omp_map_offcache_iterator(off, i, &inherited);
			if (inherited) continue;
			omp_data_map_info_t *map_info = map->info;
			if (!map_info->remap_needed) {
				misc_event_index ++;
				continue;
			}
			if (map_info->map_direction == OMP_DATA_MAP_TO || map_info->map_direction == OMP_DATA_MAP_TOFROM) {
#if defined (OMP_BREAKDOWN_TIMING)
				omp_event_t * ev = &events[misc_event_index];
				if (ev->event_name == NULL) omp_event_set_attribute(ev, off->stream, "MAPTO_", "Time for mapto data movement for array %s", map_info->symbol);
				omp_event_record_start(&events[misc_event_index]);
#endif
				omp_map_mapto_async(map, off->stream);
				//omp_map_memcpy_to_async((void*)map->map_dev_ptr, dev, (void*)map->map_buffer, map->map_size, off->stream); /* memcpy from host to device */
#if defined (OMP_BREAKDOWN_TIMING)
				omp_event_record_stop(&events[misc_event_index++]);
#endif
			}
		}
#if defined (OMP_BREAKDOWN_TIMING)
		if (off_info->num_mapped_vars > 0)
			omp_event_record_stop(&events[acc_mapto_event_index]);
#endif
	}

	off->stage = OMP_OFFLOADING_KERNEL;

//	case OMP_OFFLOADING_KERNEL:
	{
#if defined (OMP_BREAKDOWN_TIMING)
		omp_event_record_start(&events[acc_kernel_exe_event_index]);
#endif
		/* launching the kernel */
		void *args = off_info->args;
		void (*kernel_launcher)(omp_offloading_t *, void *) = off_info->kernel_launcher;
		if (args == NULL) args = off->args;
		if (kernel_launcher == NULL) kernel_launcher = off->kernel_launcher;
		kernel_launcher(off, args);
		off->loop_dist_done = 0; /* reset for the next dist if there is */
#if defined (OMP_BREAKDOWN_TIMING)
		omp_event_record_stop(&events[acc_kernel_exe_event_index]);
#endif
	}

//	case OMP_OFFLOADING_EXCHANGE:
	data_exchange:;
	/* for data exchange, either a standalone or an appended exchange */
	if (off_info->halo_x_info != NULL) {
		omp_stream_sync(
				off->stream);/* make sure previous operation are complete, should NOT be timed for exchange */
#if defined (OMP_BREAKDOWN_TIMING)
		omp_event_record_start(&events[acc_ex_pre_barrier_event_index]);
#endif
		pthread_barrier_wait(
				&off_info->inter_dev_barrier); /* make sure everybody is completed so we can exchange now */
#if defined (OMP_BREAKDOWN_TIMING)
		omp_event_record_stop(&events[acc_ex_pre_barrier_event_index]);
#endif

#if defined (OMP_BREAKDOWN_TIMING)
		omp_event_record_start(&events[acc_ex_event_index]);
#endif
		for (i = 0; i < off_info->num_maps_halo_x; i++) {
			omp_data_map_halo_exchange_info_t *x_halos = &off_info->halo_x_info[i];
			omp_data_map_info_t *map_info = x_halos->map_info;
			//int devseqid = omp_grid_topology_get_seqid(map_info->top, dev->id);

			omp_data_map_t *map = &map_info->maps[seqid];
			omp_halo_region_pull(map, x_halos->x_dim, x_halos->x_direction);
		}
#if defined (OMP_BREAKDOWN_TIMING)
		omp_event_record_stop(&events[acc_ex_event_index]);
		omp_event_record_start(&events[acc_ex_post_barrier_event_index]);
#endif
//		dev->offload_request = NULL; /* release this dev */
		pthread_barrier_wait(&off_info->inter_dev_barrier);
		//printf("dev: %d (seqid: %d) holo region pull\n", dev->id, seqid);

#if defined (OMP_BREAKDOWN_TIMING)
		omp_event_record_stop(&events[acc_ex_post_barrier_event_index]);
#endif
//		if (off_info->type == OMP_OFFLOADING_STANDALONE_DATA_EXCHANGE) goto omp_offloading_sync_cleanup;
	}

//	case OMP_OFFLOADING_COPYFROM:
	{
		omp_offloading_copyfrom:;
		off->stage = OMP_OFFLOADING_COPYFROM;
#if defined (OMP_BREAKDOWN_TIMING)
		if (off_info->num_mapped_vars > 0)
			omp_event_record_start(&events[acc_mapfrom_event_index]);
#endif
		/* copy back results */
		for (i = 0; i < off->num_maps; i++) {
			int inherited;
			omp_data_map_t *map = omp_map_offcache_iterator(off, i, &inherited);
			if (inherited) continue;
			omp_data_map_info_t *map_info = map->info;
			if (!map_info->remap_needed) {
				misc_event_index ++;
				continue;
			}
			if (map_info->map_direction == OMP_DATA_MAP_FROM || map_info->map_direction == OMP_DATA_MAP_TOFROM) {
#if defined (OMP_BREAKDOWN_TIMING)
				/* TODO bug here if this is reached from the above goto, since events is not available */
				omp_event_t * ev = &events[misc_event_index];
				if (ev->event_name == NULL) omp_event_set_attribute(ev, off->stream, "MAPFROM_", "Time for mapfrom data movement for array %s", map_info->symbol);
				omp_event_record_start(&events[misc_event_index]);
#endif
				omp_map_mapfrom_async(map, off->stream);
				//omp_map_memcpy_from_async((void*)map->map_buffer, (void*)map->map_dev_ptr, dev, map->map_size, off->stream); /* memcpy from host to device */
#if defined (OMP_BREAKDOWN_TIMING)
				omp_event_record_stop(&events[misc_event_index++]);
#endif
			}
		}
#if defined (OMP_BREAKDOWN_TIMING)
		if (off_info->num_mapped_vars > 0)
			omp_event_record_stop(&events[acc_mapfrom_event_index]);
#endif
	}
}

void omp_accumulate_elapsed_ms (omp_event_t * events, int num_events) {
	int i;
	double accu_time = 0.0;
	omp_event_accumulate_elapsed_ms(&events[total_event_index], 0);
	omp_event_accumulate_elapsed_ms(&events[timing_init_event_index], 0);
	omp_event_accumulate_elapsed_ms(&events[map_init_event_index], 0);
	omp_event_accumulate_elapsed_ms(&events[map_dist_alloc_event_index], 0);
	omp_event_accumulate_elapsed_ms(&events[runtime_dist_modeling_index], 0);
	omp_event_accumulate_elapsed_ms(&events[sync_cleanup_event_index], 0);
	omp_event_accumulate_elapsed_ms(&events[barrier_wait_event_index], 0);
	accu_time += omp_event_accumulate_elapsed_ms(&events[acc_mapto_event_index], 0);
	accu_time += omp_event_accumulate_elapsed_ms(&events[acc_kernel_exe_event_index], 0);
	accu_time += omp_event_accumulate_elapsed_ms(&events[acc_mapfrom_event_index], 0);
	accu_time += omp_event_accumulate_elapsed_ms(&events[acc_ex_pre_barrier_event_index], 0);
	accu_time += omp_event_accumulate_elapsed_ms(&events[acc_ex_event_index], 0);
	accu_time += omp_event_accumulate_elapsed_ms(&events[acc_ex_post_barrier_event_index], 0);
	for (i=misc_event_index_start; i<num_events; i++) {
		omp_event_accumulate_elapsed_ms(&events[i], 0);
	}
	omp_event_record_start(&events[total_event_accumulated_index]);
	omp_event_record_stop(&events[total_event_accumulated_index]);
	omp_event_accumulate_elapsed_ms(&events[total_event_accumulated_index], accu_time);
}

/**
 * called by the shepherd thread
 */
void omp_offloading_run(omp_device_t * dev) {
	omp_offloading_info_t * off_info = dev->offload_request;
	omp_grid_topology_t * top = off_info->top;
	int seqid = omp_grid_topology_get_seqid(top, dev->id); /* we assume this is tiny for timing, so not included */
	omp_offloading_t * off = &off_info->offloadings[seqid];
	//printf("devid: %d --> seqid: %d in top: %X, off: %X, off_info: %X\n", dev->id, seqid, top, off, off_info);

	int devid = dev->id;
	int i = 0;

#if defined (OMP_BREAKDOWN_TIMING)
	/* the num_mapped_vars * 2 +4 is the rough number of events needed */
	/* the event (if mapto var is num_mapto, and mapfrom var is num_mapfrom (both including tofrom);
	 * 0: The whole measured time from host side, measured from host
	 * 1: The init time (stream, event, etc), this is the overhead for the breakdown timing, measured from host
	 * 2: The time for map init, data dist, buffer allocation and data marshalling, measured from host
	 * 3: The accumulated time for mapto datamovement, measured from dev
	 * 4 - acc_kernel_exe_event_index-1: The time for each mapto datamovement, measured from dev (total num_mapto events)
	 * acc_kernel_exe_event_index: kernel exe time
	 * acc_kernel_exe_event_index+1: The accumulated time for mapfrom datamovement, measured from dev
	 *     acc_kernel_exe_event_index+2 - xxxx: The time for each mapfrom datamovement, measured from dev (total num_mapfrom events)
	 * xxxx: The time for cleanup resources (stream, event, data unmarshalling, etc), measured from host
	 * xxxx: The time for barrier wait (for other kernel to complete), measured from host
	 */

	int num_events;
	omp_event_t *events;
	if (off_info->count <= 1) { /* the first time of recurring offloading or a non-recurring offloading */
		num_events = off_info->num_mapped_vars * 2 + misc_event_index_start; /* the max posibble # of events to be used */
		events = (omp_event_t *) malloc(sizeof(omp_event_t) * num_events); /**TODO: free this memory somewhere later */
		off->num_events = num_events;
		off->events = events;
	} else { /* second time and later recurring offloading */
		num_events = off->num_events;
		events = off->events;
	}

	/* set up stream and event */
	if (off_info->count <= 1)
		omp_event_init(&events[total_event_index], dev, OMP_EVENT_HOST_RECORD, NULL, "OFF_TOTAL", "Total offloading time (everything) on dev: %d", devid);
	omp_event_record_start(&events[total_event_index]);
#endif

//	case OMP_OFFLOADING_INIT:
	if (off_info->count <= 1) { /* the first time of recurring offloading or a non-recurring offloading */
#if defined USING_PER_OFFLOAD_STREAM
		omp_stream_create(dev, &off->mystream);
		off->stream = &off->mystream;
#else
		off->stream = &dev->default_stream;
#endif
		omp_dev_stream_t *stream = off->stream;

#if defined (OMP_BREAKDOWN_TIMING)
		omp_event_init(&events[timing_init_event_index], dev, OMP_EVENT_HOST_RECORD, NULL, "INIT_0", "Time for initialization of stream and event", devid);
		omp_event_record_start(&events[timing_init_event_index]);
		omp_event_init(&events[total_event_accumulated_index], dev, OMP_EVENT_HOST_RECORD, NULL, "ACCU_TOTAL", "Total ACCUMULATED time on dev: %d", devid);
		omp_event_init(&events[map_init_event_index], dev, OMP_EVENT_HOST_RECORD, NULL, "INIT_0.1", "Time for init map data structure");
		omp_event_init(&events[map_dist_alloc_event_index], dev, OMP_EVENT_HOST_RECORD, NULL, "INIT_1", "Time for data dist, memory allocation, and data marshalling");
		omp_event_init(&events[runtime_dist_modeling_index], dev, OMP_EVENT_HOST_RECORD, NULL, "MODELING", "Runtime modeling cost");
		omp_event_init(&events[sync_cleanup_event_index], dev, OMP_EVENT_HOST_RECORD, NULL, "FINI_1", "Time for dev sync and cleaning (event/stream/map, deallocation/unmarshalling)");
		omp_event_init(&events[barrier_wait_event_index], dev, OMP_EVENT_HOST_RECORD, NULL, "BAR_FINI_2", "Time for barrier wait for other to complete");
		omp_event_init(&events[acc_mapto_event_index], dev, OMP_EVENT_DEV_RECORD, stream, "ACC_MAPTO", "Accumulated time for mapto data movement for all array");
		omp_event_init(&events[acc_kernel_exe_event_index], dev, OMP_EVENT_DEV_RECORD, stream, "KERN", "Time for kernel (%s) execution", off_info->name);
		omp_event_init(&events[acc_mapfrom_event_index], dev, OMP_EVENT_DEV_RECORD, stream, "ACC_MAPFROM", "Accumulated time for mapfrom data movement for all array");
		omp_event_init(&events[acc_ex_pre_barrier_event_index], dev, OMP_EVENT_HOST_RECORD, NULL, "PRE_BAR_X", "Time for barrier sync before data exchange between devices");
		omp_event_init(&events[acc_ex_event_index], dev, OMP_EVENT_HOST_RECORD, NULL, "DATA_X", "Time for data exchange between devices");
		omp_event_init(&events[acc_ex_post_barrier_event_index], dev, OMP_EVENT_HOST_RECORD, NULL, "POST_BAR_X", "Time for barrier sync after data exchange between devices");

		for (i=misc_event_index_start; i<num_events; i++) {
			omp_event_init(&events[i], dev, OMP_EVENT_DEV_RECORD, NULL, NULL, NULL, 0);
		}

		omp_event_record_stop(&events[timing_init_event_index]);
#endif
#if defined (OMP_BREAKDOWN_TIMING)
		omp_event_record_start(&events[map_init_event_index]);
#endif
		for (i = 0; i < off_info->num_mapped_vars; i++) {
			/* we handle inherited map here, by each helper thread, and we only update the off object (not off_info)*/
			omp_data_map_info_t *map_info = &off_info->data_map_info[i];

			int inherited = 1;
			omp_data_map_t *map = omp_map_get_map_inheritance(dev, map_info->source_ptr);
			if (map == NULL) { /* here we basically ignore any map specification if it can inherit from ancestor (upper level nested target data) */
				map = &map_info->maps[seqid];
				omp_data_map_init_map(map, map_info, dev);
				inherited = 0;
			}
			omp_map_append_map_to_offcache(off, map, inherited);
			//omp_print_data_map(map);
		}
#if defined (OMP_BREAKDOWN_TIMING)
		omp_event_record_stop(&events[map_init_event_index]);
#endif
	}

	int misc_event_index = misc_event_index_start;
#if defined (OMP_BREAKDOWN_TIMING)
	omp_event_record_start(&events[map_dist_alloc_event_index]);
#endif
	if (!off->loop_dist_done) omp_loop_iteration_dist(off);

	//case OMP_OFFLOADING_MAPMEM:
	off->stage = OMP_OFFLOADING_MAPMEM;
	/* init data map and dev memory allocation */
	/***************** for each mapped variable that has to and tofrom, if it has region mapped to this __ndev_i__ id, we need code here *******************************/
	for (i = 0; i < off_info->num_mapped_vars; i++) {
		/* we handle inherited map here, by each helper thread, and we only update the off object (not off_info)*/
		omp_data_map_info_t *map_info = &off_info->data_map_info[i];

		int inherited = 1;
		omp_data_map_t *map = omp_map_offcache_iterator(off, i, &inherited);
		if (inherited) continue;
		omp_data_map_dist(map, seqid); /* handle all unmapped variable */
		if (map->access_level != OMP_DATA_MAP_ACCESS_LEVEL_MALLOC) omp_map_malloc(map, off);
		//omp_print_data_map(map);
	}

#if defined (OMP_BREAKDOWN_TIMING)
	omp_event_record_stop(&events[map_dist_alloc_event_index]);
#endif

	if (off_info->type == OMP_OFFLOADING_STANDALONE_DATA_EXCHANGE) goto data_exchange;
	if (off_info->type == OMP_OFFLOADING_DATA && off_info->count > 1) {
		goto omp_offloading_copyfrom;
	}

//	case OMP_OFFLOADING_COPYTO:
	{
		omp_offloading_copyto:;
		off->stage = OMP_OFFLOADING_COPYTO;
#if defined (OMP_BREAKDOWN_TIMING)
		if (off_info->num_mapped_vars > 0)
			omp_event_record_start(&events[acc_mapto_event_index]);
#endif
		for (i = 0; i < off->num_maps; i++) {
			int inherited;
			omp_data_map_t *map = omp_map_offcache_iterator(off, i, &inherited);
			if (inherited) continue;
			omp_data_map_info_t *map_info = map->info;
			if (map_info->map_direction == OMP_DATA_MAP_TO || map_info->map_direction == OMP_DATA_MAP_TOFROM) {
#if defined (OMP_BREAKDOWN_TIMING)
				omp_event_t * ev = &events[misc_event_index];
				if (ev->event_name == NULL) omp_event_set_attribute(ev, off->stream, "MAPTO_", "Time for mapto data movement for array %s", map_info->symbol);
				omp_event_record_start(&events[misc_event_index]);
#endif
				omp_map_mapto_async(map, off->stream);
				//omp_map_memcpy_to_async((void*)map->map_dev_ptr, dev, (void*)map->map_buffer, map->map_size, off->stream); /* memcpy from host to device */
#if defined (OMP_BREAKDOWN_TIMING)
				omp_event_record_stop(&events[misc_event_index++]);
#endif
			}
		}
#if defined (OMP_BREAKDOWN_TIMING)
		if (off_info->num_mapped_vars > 0)
			omp_event_record_stop(&events[acc_mapto_event_index]);
#endif
	}

	if (off_info->type == OMP_OFFLOADING_DATA) { /* only data offloading, i.e., OMP_OFFLOADING_DATA */
		//assert (off_info->recurring == 1);
		off->stage = OMP_OFFLOADING_SYNC;
		return; //goto omp_offloading_sync_cleanup;
	} else {
		off->stage = OMP_OFFLOADING_KERNEL;
	}

//	case OMP_OFFLOADING_KERNEL:
	{
#if defined (OMP_BREAKDOWN_TIMING)
		omp_event_record_start(&events[acc_kernel_exe_event_index]);
#endif
		/* launching the kernel */
		void *args = off_info->args;
		void (*kernel_launcher)(omp_offloading_t *, void *) = off_info->kernel_launcher;
		if (args == NULL) args = off->args;
		if (kernel_launcher == NULL) kernel_launcher = off->kernel_launcher;
		kernel_launcher(off, args);
		off->loop_dist_done = 0; /* reset for the next if there is */
#if defined (OMP_BREAKDOWN_TIMING)
		omp_event_record_stop(&events[acc_kernel_exe_event_index]);
#endif
	}

//	case OMP_OFFLOADING_EXCHANGE:
	data_exchange:;
	/* for data exchange, either a standalone or an appended exchange */
	if (off_info->halo_x_info != NULL) {
		omp_stream_sync(
				off->stream);/* make sure previous operation are complete, should NOT be timed for exchange */
#if defined (OMP_BREAKDOWN_TIMING)
		omp_event_record_start(&events[acc_ex_pre_barrier_event_index]);
#endif
		pthread_barrier_wait(
				&off_info->inter_dev_barrier); /* make sure everybody is completed so we can exchange now */
#if defined (OMP_BREAKDOWN_TIMING)
		omp_event_record_stop(&events[acc_ex_pre_barrier_event_index]);
#endif

#if defined (OMP_BREAKDOWN_TIMING)
		omp_event_record_start(&events[acc_ex_event_index]);
#endif
		for (i = 0; i < off_info->num_maps_halo_x; i++) {
			omp_data_map_halo_exchange_info_t *x_halos = &off_info->halo_x_info[i];
			omp_data_map_info_t *map_info = x_halos->map_info;
			//int devseqid = omp_grid_topology_get_seqid(map_info->top, dev->id);

			omp_data_map_t *map = &map_info->maps[seqid];
			omp_halo_region_pull(map, x_halos->x_dim, x_halos->x_direction);
		}
#if defined (OMP_BREAKDOWN_TIMING)
		omp_event_record_stop(&events[acc_ex_event_index]);
		omp_event_record_start(&events[acc_ex_post_barrier_event_index]);
#endif
//		dev->offload_request = NULL; /* release this dev */
		pthread_barrier_wait(&off_info->inter_dev_barrier);
		//printf("dev: %d (seqid: %d) holo region pull\n", dev->id, seqid);

#if defined (OMP_BREAKDOWN_TIMING)
		omp_event_record_stop(&events[acc_ex_post_barrier_event_index]);
#endif
//		if (off_info->type == OMP_OFFLOADING_STANDALONE_DATA_EXCHANGE) goto omp_offloading_sync_cleanup;
	}

	omp_dist_policy_t loop_dist_policy = off_info->loop_dist_info[off_info->loop_depth].policy;
	if (loop_dist_policy == OMP_DIST_POLICY_SCHEDULE_STATIC || loop_dist_policy == OMP_DIST_POLICY_SCHEDULE_DYNAMIC) {
		int num_iterations = 0;
		do {
#if defined (OMP_BREAKDOWN_TIMING)
			omp_accumulate_elapsed_ms(events, num_events);
#endif
			num_iterations = secondary_offload_cycle(off_info, off, events, seqid, misc_event_index_start);
		} while(num_iterations);
	} else if (loop_dist_policy == OMP_DIST_POLICY_PROFILE_AUTO){
#if defined (OMP_BREAKDOWN_TIMING)
		omp_accumulate_elapsed_ms(events, num_events);
#endif
        /* we need barrier here to make sure every device finishes its portion for collective profiling and ratio modeling */
		pthread_barrier_wait(&off_info->inter_dev_barrier);
		secondary_offload_cycle(off_info, off, events, seqid, misc_event_index_start);
	}

//	case OMP_OFFLOADING_COPYFROM:
	{
		omp_offloading_copyfrom:;
		off->stage = OMP_OFFLOADING_COPYFROM;
#if defined (OMP_BREAKDOWN_TIMING)
		if (off_info->num_mapped_vars > 0)
			omp_event_record_start(&events[acc_mapfrom_event_index]);
#endif
		/* copy back results */
		for (i = 0; i < off->num_maps; i++) {
			int inherited;
			omp_data_map_t *map = omp_map_offcache_iterator(off, i, &inherited);
			if (inherited) continue;
			omp_data_map_info_t *map_info = map->info;
			if (map_info->map_direction == OMP_DATA_MAP_FROM || map_info->map_direction == OMP_DATA_MAP_TOFROM) {
#if defined (OMP_BREAKDOWN_TIMING)
				/* TODO bug here if this is reached from the above goto, since events is not available */
				omp_event_t * ev = &events[misc_event_index];
				if (ev->event_name == NULL) omp_event_set_attribute(ev, off->stream, "MAPFROM_", "Time for mapfrom data movement for array %s", map_info->symbol);
				omp_event_record_start(&events[misc_event_index]);
#endif
				omp_map_mapfrom_async(map, off->stream);
				//omp_map_memcpy_from_async((void*)map->map_buffer, (void*)map->map_dev_ptr, dev, map->map_size, off->stream); /* memcpy from host to device */
#if defined (OMP_BREAKDOWN_TIMING)
				omp_event_record_stop(&events[misc_event_index++]);
#endif
			}
		}
#if defined (OMP_BREAKDOWN_TIMING)
		if (off_info->num_mapped_vars > 0)
			omp_event_record_stop(&events[acc_mapfrom_event_index]);
#endif
	}

	//case OMP_OFFLOADING_SYNC:
	//case OMP_OFFLOADING_SYNC_CLEANUP:
	{
omp_offloading_sync_cleanup: ;
		/* sync stream to wait for completion */
		omp_stream_sync(off->stream); /*NOTE: we should NOT time this call as the event system already count in as previous async kernel or async memcpy */
#if defined (OMP_BREAKDOWN_TIMING)
		omp_event_record_start(&events[sync_cleanup_event_index]);
#endif
		if (off->stage == OMP_OFFLOADING_SYNC) {
			if (off_info->type == OMP_OFFLOADING_DATA) { /* this should be just an assertation */
				/* put in the offloading stack */
				dev->offload_stack_top++;
				dev->offload_stack[dev->offload_stack_top] = off;
				//printf("pushing an off %X onto offload stack at position %d\n", off, dev->offload_stack_top);
			}
		} else {
			if (off_info->type == OMP_OFFLOADING_DATA) { /* pop up this offload stack */
				dev->offload_stack_top--;
				//printf("pop an off %X onto offload stack at position %d\n", off, dev->offload_stack_top+1);
			}

			off->stage = OMP_OFFLOADING_SYNC_CLEANUP;
		}
		if (off_info->free_after_completion) {
			for (i=0; i<off->num_maps; i++) {
				int inherited;
				omp_data_map_t *map = omp_map_offcache_iterator(off, i, &inherited);
				if (!inherited) omp_map_free(map, off);
			}
#if defined USING_PER_OFFLOAD_STREAM
			omp_stream_destroy(&off->mystream);
#endif
		}
#if defined (OMP_BREAKDOWN_TIMING)
		omp_event_record_stop(&events[sync_cleanup_event_index]);
#endif
		off->stage = OMP_OFFLOADING_MDEV_BARRIER;
	}

	//	case OMP_OFFLOADING_MDEV_BARRIER:
	{
#if defined (OMP_BREAKDOWN_TIMING)
		omp_event_record_start(&events[barrier_wait_event_index]);
#endif
		dev->offload_request = NULL; /* release this dev */
		pthread_barrier_wait(&off_info->barrier);
#if defined (OMP_BREAKDOWN_TIMING)
		omp_event_record_stop(&events[barrier_wait_event_index]);
#endif
		//off_info->stage = OMP_OFFLOADING_COMPLETE; /* data race for any access to off_info
	}

#if defined (OMP_BREAKDOWN_TIMING)
	omp_event_record_stop(&events[total_event_index]);
#endif

	/* print out the timing info */
#if defined (OMP_BREAKDOWN_TIMING)
	/* do timing accumulation if this is a recurring kernel */
	omp_accumulate_elapsed_ms(events, num_events);
	pthread_barrier_wait(&off_info->barrier);
#endif
}

/* helper thread main */
void helper_thread_main(void * arg) {
	omp_device_t * dev = (omp_device_t*)arg;

	omp_set_current_device_dev(dev);
	omp_warmup_device(dev);
	omp_stream_create(dev, &dev->default_stream);
	omp_stream_sync(&dev->default_stream);
//	omp_set_num_threads(dev->num_cores);
	pthread_barrier_wait(&all_dev_sync_barrier);
//	printf("helper threading (devid: %s) loop ....\n", dev->name);
	/*************** loop *******************/
	while (omp_device_complete == 0) {
//		printf("helper threading (devid: %X) waiting ....\n", dev);
		while (dev->offload_request == NULL) {
			if (omp_device_complete) return;
		}
//		printf("helper threading (devid: %X) offloading  ....\n", dev);
		omp_offloading_run(dev);
	}

	omp_stream_destroy(&dev->default_stream);
}
