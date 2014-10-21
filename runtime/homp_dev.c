/*
 * homp_dev.c
 *
 * contains dev-specific implementation of homp.h functions, mainly those with
 *
 *  Created on: Oct 4, 2014
 *      Author: yy8
 */
/**
 * an easy way for defining dev-specific code:
#if defined (DEVICE_NVGPU_SUPPORT)

#elif defined (DEVICE_THSIM)

#else

#endif
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "homp.h"

inline void devcall_errchk(int code, char *file, int line, int abort) {
#if defined (DEVICE_NVGPU_SUPPORT)
	if (code != cudaSuccess) {
		fprintf(stderr,"GPUassert: %s %s %d\n", cudaGetErrorString(code), file, line);
		if (abort) exit(code);
	}
#elif defined (DEVICE_THSIM)
	if (code != 0) {
		fprintf(stderr, "devcal_assert: %d %s %d\n", code, file, line);
		if (abort) exit(code);
	}
#endif
}

/* init the device objects, num_of_devices, default_device_var ICV etc
 *
 */
int omp_init_devices() {
	/* query hardware device */
	omp_num_devices = 0;

	/* the thread-simulated devices */
	int num_thsim_dev;

	char * num_thsim_dev_str = getenv("OMP_NUM_THSIM_DEVICES");
	if (num_thsim_dev_str != NULL ) {
		sscanf(num_thsim_dev_str, "%d", &num_thsim_dev);
		if (num_thsim_dev < 0) num_thsim_dev = 0;
	} else num_thsim_dev = 0;

	omp_num_devices += num_thsim_dev;

	/* for NVDIA GPU devices */
	int total_gpudevs = 0;
#if defined (DEVICE_NVGPU_SUPPORT)
	cudaError_t result = cudaGetDeviceCount(&total_gpudevs);
	devcall_assert(result);
#endif
	/* query other type of device */

	int num_nvgpu_dev;
	char * num_nvgpu_dev_str = getenv("OMP_NUM_NVGPU_DEVICES");
	if (num_nvgpu_dev_str != NULL ) {
		sscanf(num_nvgpu_dev_str, "%d", &num_nvgpu_dev);
		if (num_nvgpu_dev > total_gpudevs || num_nvgpu_dev < 0) num_nvgpu_dev = total_gpudevs;
	} else num_nvgpu_dev = total_gpudevs;

	omp_num_devices += num_nvgpu_dev;

	omp_devices = malloc(sizeof(omp_device_t) * omp_num_devices);
	int i;

	/* the helper thread setup */
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	/* initialize attr with default attributes */
	pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
	pthread_setconcurrency(omp_num_devices);

	for (i=0; i<omp_num_devices; i++) {
		omp_device_t * dev = &omp_devices[i];
		dev->id = i;
		if (i < num_nvgpu_dev) dev->type = OMP_DEVICE_NVGPU;
		else dev->type  = OMP_DEVICE_THSIM;
		dev->status = 1;
		dev->sysid = i;
		dev->resident_data_maps = NULL;
		dev->next = &omp_devices[i+1];
		dev->offload_request = NULL;
		dev->data_exchange_request = NULL;
		dev->offload_stack_top = -1;

		int rt = pthread_create(&dev->helperth, &attr, (void *(*)(void *))helper_thread_main, (void *) dev);
		if (rt) {fprintf(stderr, "cannot create helper threads for devices.\n"); exit(1); }
	}
	if (omp_num_devices) {
		default_device_var = 0;
		omp_devices[omp_num_devices-1].next = NULL;
	}
	printf("System has total %d devices(%d GPU and %d THSIM devices).\n", omp_num_devices, num_nvgpu_dev, num_thsim_dev);
	printf("The number of each type of devices can be controlled by environment variables:\n");
	printf("\tOMP_NUM_THSIM_DEVICES for THSIM devices (default 0)\n");
	printf("\tOMP_NUM_NVGPU_DEVICES for active NVIDIA GPU devices (default, system available)\n");
	printf("\tTo make a specific number of devices available, use OMP_NUM_ACTIVE_DEVICES (default, total number of system devices)\n");
	return omp_num_devices;

}

int omp_set_current_device_dev(omp_device_t * d) {
#if defined (DEVICE_NVGPU_SUPPORT)
    int result;
	if (d->type == OMP_DEVICE_NVGPU) {
		result = cudaSetDevice(d->sysid);
		devcall_assert (result);
	}
#endif
	return d->id;
}

void * omp_map_malloc_dev(omp_device_t * dev, long size) {
	omp_device_type_t devtype = dev->type;
	void * ptr = NULL;
#if defined (DEVICE_NVGPU_SUPPORT)
	if (devtype == OMP_DEVICE_NVGPU) {
		if (cudaErrorMemoryAllocation == cudaMalloc(&ptr, size)) {
			fprintf(stderr, "cudaMalloc error to allocate mem on device\n");
		}
	} else
#endif
	if (devtype == OMP_DEVICE_THSIM) {
		ptr = malloc(size);
	} else {
		fprintf(stderr, "device type is not supported for this call\n");
	}
	return ptr;
}

void omp_map_free_dev(omp_device_t * dev, void * ptr) {
	omp_device_type_t devtype = dev->type;
#if defined (DEVICE_NVGPU_SUPPORT)
	if (devtype == OMP_DEVICE_NVGPU) {
	    cudaError_t result = cudaFree(ptr);
	    devcall_assert(result);
	} else
#endif
	if (devtype == OMP_DEVICE_THSIM) {
		free(ptr);
	} else {
		fprintf(stderr, "device type is not supported for this call\n");
	}
}

void omp_map_memcpy_to(void * dst, omp_device_t * dstdev, const void * src, long size) {
	omp_device_type_t devtype = dstdev->type;
#if defined (DEVICE_NVGPU_SUPPORT)
	if (devtype == OMP_DEVICE_NVGPU) {
	    cudaError_t result;
	    result = cudaMemcpy((void *)dst,(const void *)src,size, cudaMemcpyHostToDevice);
	    devcall_assert(result);
	} else
#endif
	if (devtype == OMP_DEVICE_THSIM) {
		memcpy((void *)dst,(const void *)src,size);
	} else {
		fprintf(stderr, "device type is not supported for this call\n");
	}
}

void omp_map_memcpy_to_async(void * dst, omp_device_t * dstdev, const void * src, long size, omp_dev_stream_t * stream) {
	omp_device_type_t devtype = dstdev->type;
#if defined (DEVICE_NVGPU_SUPPORT)
	if (devtype == OMP_DEVICE_NVGPU) {
		cudaError_t result;
		result = cudaMemcpyAsync((void *)dst,(const void *)src,size, cudaMemcpyHostToDevice, stream->systream.cudaStream);
		devcall_assert(result);
	} else
#endif
	if (devtype == OMP_DEVICE_THSIM) {
//		fprintf(stderr, "no async call support, use sync memcpy call\n");
		memcpy((void *)dst,(const void *)src,size);
	} else {
		fprintf(stderr, "device type is not supported for this call\n");
	}
}

void omp_map_memcpy_from(void * dst, const void * src, omp_device_t * srcdev, long size) {
	omp_device_type_t devtype = srcdev->type;
#if defined (DEVICE_NVGPU_SUPPORT)
	if (devtype == OMP_DEVICE_NVGPU) {
		cudaError_t result;
	    result = cudaMemcpy((void *)dst,(const void *)src,size, cudaMemcpyDeviceToHost);
		devcall_assert(result);
	} else
#endif
	if (devtype == OMP_DEVICE_THSIM) {
		memcpy((void *)dst,(const void *)src,size);
	} else {
		fprintf(stderr, "device type is not supported for this call\n");
	}
}

/**
 *  device to host, async */
void omp_map_memcpy_from_async(void * dst, const void * src, omp_device_t * srcdev, long size, omp_dev_stream_t * stream) {
	omp_device_type_t devtype = srcdev->type;
#if defined (DEVICE_NVGPU_SUPPORT)
	if (devtype == OMP_DEVICE_NVGPU) {
		cudaError_t result;
		result = cudaMemcpyAsync((void *)dst,(const void *)src,size, cudaMemcpyDeviceToHost, stream->systream.cudaStream);
		devcall_assert(result);
	} else
#endif
	if (devtype == OMP_DEVICE_THSIM) {
//		fprintf(stderr, "no async call support, use sync memcpy call\n");
		memcpy((void *)dst,(const void *)src,size);
//		printf("memcpy from: dest: %X, src: %X, size: %d\n", map->map_buffer, map->map_dev_ptr);
	} else {
		fprintf(stderr, "device type is not supported for this call\n");
	}
}

/**
 * this should be calling from src for NGVPU implementation
 */
int omp_map_enable_memcpy_DeviceToDevice(omp_device_t * dstdev, omp_device_t * srcdev) {
	omp_device_type_t dst_devtype = dstdev->type;
	omp_device_type_t src_devtype = srcdev->type;

#if defined (DEVICE_NVGPU_SUPPORT)
	if (dst_devtype == OMP_DEVICE_NVGPU && src_devtype == OMP_DEVICE_NVGPU) {
		int can_access = 0;
		cudaError_t result;
		result = cudaDeviceCanAccessPeer(&can_access, srcdev->sysid, dstdev->sysid);
		devcall_assert(result);
		if (can_access) {
			result = cudaDeviceEnablePeerAccess(dstdev->sysid, 0);
		    if(result != cudaErrorPeerAccessAlreadyEnabled) {
		    	return 0;
		    } else return 1;
		} else return 1;
	} else
#endif
	if (dst_devtype == OMP_DEVICE_THSIM && src_devtype == OMP_DEVICE_THSIM) {
		return 1;
	} else {
		fprintf(stderr, "device type is not supported for this call, currently we only support p2p copy between GPU-GPU and TH-TH\n");
	}
	return 0;
}

void omp_map_memcpy_DeviceToDevice(void * dst, omp_device_t * dstdev, void * src, omp_device_t * srcdev, int size) {
	omp_device_type_t dst_devtype = dstdev->type;
	omp_device_type_t src_devtype = srcdev->type;

#if defined (DEVICE_NVGPU_SUPPORT)
	if (dst_devtype == OMP_DEVICE_NVGPU && src_devtype == OMP_DEVICE_NVGPU) {
		cudaError_t result;
	    result = cudaMemcpy((void *)dst,(const void *)src,size, cudaMemcpyDeviceToDevice);
//		result = cudaMemcpyPeer(dst, dstdev->sysid, src, srcdev->sysid, size);
		devcall_assert(result);
	} else
#endif
	if (dst_devtype == OMP_DEVICE_THSIM && src_devtype == OMP_DEVICE_THSIM) {
		memcpy((void *)dst, (const void *)src, size);
	} else {
		fprintf(stderr, "device type is not supported for this call, currently we only support p2p copy between GPU-GPU and TH-TH\n");
	}
}

/** it is a push operation, i.e. src push data to dst */
void omp_map_memcpy_DeviceToDeviceAsync(void * dst, omp_device_t * dstdev, void * src, omp_device_t * srcdev, int size, omp_dev_stream_t * srcstream) {
	omp_device_type_t dst_devtype = dstdev->type;
	omp_device_type_t src_devtype = srcdev->type;

#if defined (DEVICE_NVGPU_SUPPORT)
	if (dst_devtype == OMP_DEVICE_NVGPU && src_devtype == OMP_DEVICE_NVGPU) {
		cudaError_t result;
	    result = cudaMemcpyAsync((void *)dst,(const void *)src,size, cudaMemcpyDeviceToDevice,srcstream->systream.cudaStream);
	    //result = cudaMemcpyPeerAsync(dst, dstdev->sysid, src, srcdev->sysid, size, srcstream->systream.cudaStream);

		devcall_assert(result);
	} else
#endif
	if (dst_devtype == OMP_DEVICE_THSIM && src_devtype == OMP_DEVICE_THSIM) {
		memcpy((void *)dst, (const void *)src, size);
	} else {
		fprintf(stderr, "device type is not supported for this call, currently we only support p2p copy between GPU-GPU and TH-TH\n");
	}
}

#if defined (DEVICE_NVGPU_SUPPORT)
void xomp_beyond_block_reduction_float_stream_callback(cudaStream_t stream,  cudaError_t status, void*  userData ) {
	omp_reduction_float_t * rdata = (omp_reduction_float_t*)userData;
	float result = 0.0;
	int i;
	for (i=0; i<rdata->num; i++)
		result += rdata->input[i];
	rdata->result = result;
}


#ifdef USE_STREAM_HOST_CALLBACK_4_TIMING
void omp_stream_host_timer_callback(cudaStream_t stream,  cudaError_t status, void*  userData ) {
	float * time = (float*)userData;
	*time = read_timer_ms();
}
#endif
#endif

void omp_init_stream(omp_device_t * d, omp_dev_stream_t * stream) {
	stream->dev = d;
	int i;

#if defined (DEVICE_NVGPU_SUPPORT)
	cudaError_t result;
	if (d->type == OMP_DEVICE_NVGPU) {
		result = cudaStreamCreate(&stream->systream.cudaStream);
		devcall_assert(result);
	} else
#endif
	if (d->type == OMP_DEVICE_THSIM){
		/* do nothing */
	} else {

	}
}

void omp_event_init(omp_event_t * ev, omp_dev_stream_t * stream, omp_event_record_method_t record_method) {
	ev->stream = stream;
	ev->record_method = record_method;
	omp_device_type_t devtype = stream->dev->type;
	ev->elapsed_dev = ev->elapsed_host = 0.0;
	if (record_method == OMP_EVENT_DEV_RECORD || record_method == OMP_EVENT_HOST_DEV_RECORD) {
#if defined (DEVICE_NVGPU_SUPPORT)
		if (devtype == OMP_DEVICE_NVGPU) {
			cudaError_t result;
			result = cudaEventCreateWithFlags(&ev->start_event_dev, cudaEventBlockingSync);
			devcall_assert(result);
			result = cudaEventCreateWithFlags(&ev->stop_event_dev, cudaEventBlockingSync);
			devcall_assert(result);
		} else
#endif
		if (devtype == OMP_DEVICE_THSIM) {
			/* do nothing */
		} else {
			fprintf(stderr, "other type of devices are not yet supported\n");
		}
	}
}

void omp_event_record_start(omp_event_t * ev) {
	omp_dev_stream_t * stream = ev->stream;
	omp_event_record_method_t record_method = ev->record_method;
	omp_device_type_t devtype = stream->dev->type;
	if (record_method == OMP_EVENT_DEV_RECORD || record_method == OMP_EVENT_HOST_DEV_RECORD) {
#if defined (DEVICE_NVGPU_SUPPORT)
		if (devtype == OMP_DEVICE_NVGPU) {
			cudaError_t result;
#ifdef USE_STREAM_HOST_CALLBACK_4_TIMING
			result = cudaStreamAddCallback(stream->systream.cudaStream, omp_stream_host_timer_callback, &ev->start_time_dev, 0);
#else
			result = cudaEventRecord(ev->start_event_dev, stream->systream.cudaStream);
#endif
			devcall_assert(result);
		} else
#endif
		if (devtype == OMP_DEVICE_THSIM) {
			ev->start_time_dev = read_timer_ms();
		} else {
			fprintf(stderr, "other type of devices are not yet supported\n");
		}
	}

	if (record_method == OMP_EVENT_HOST_RECORD || record_method == OMP_EVENT_HOST_DEV_RECORD) {
		ev->start_time_host = read_timer_ms();
	}
}

void omp_event_record_stop(omp_event_t * ev) {
	omp_dev_stream_t * stream = ev->stream;
	omp_event_record_method_t record_method = ev->record_method;
	omp_device_type_t devtype = stream->dev->type;
	if (record_method == OMP_EVENT_DEV_RECORD || record_method == OMP_EVENT_HOST_DEV_RECORD) {
#if defined (DEVICE_NVGPU_SUPPORT)
		if (devtype == OMP_DEVICE_NVGPU) {
			cudaError_t result;
#ifdef USE_STREAM_HOST_CALLBACK_4_TIMING
			result = cudaStreamAddCallback(stream->systream.cudaStream, omp_stream_host_timer_callback, &ev->stop_time_dev, 0);
#else
			result = cudaEventRecord(ev->stop_event_dev, stream->systream.cudaStream);
#endif
			devcall_assert(result);
		} else
#endif
		if (devtype == OMP_DEVICE_THSIM) {
			ev->stop_time_dev = read_timer_ms();
		} else {
			fprintf(stderr, "other type of devices are not yet supported\n");
		}
	}

	if (record_method == OMP_EVENT_HOST_RECORD || record_method == OMP_EVENT_HOST_DEV_RECORD) {
		ev->stop_time_host = read_timer_ms();
	}
}

/**
 * Computes the elapsed time between two events (in milliseconds with a resolution of around 0.5 microseconds).
 */
void omp_event_elapsed_ms(omp_event_t * ev) {
	omp_dev_stream_t * stream = ev->stream;
	omp_event_record_method_t record_method = ev->record_method;
	omp_device_type_t devtype = stream->dev->type;
	float elapse;
	if (record_method == OMP_EVENT_DEV_RECORD || record_method == OMP_EVENT_HOST_DEV_RECORD) {
#if defined (DEVICE_NVGPU_SUPPORT)
		if (devtype == OMP_DEVICE_NVGPU) {
#ifdef USE_STREAM_HOST_CALLBACK_4_TIMING
			elapse = ev->stop_time_dev - ev->start_time_dev;
#else
			cudaError_t result;
			result = cudaEventSynchronize(ev->start_event_dev);
			devcall_assert(result);
			result = cudaEventSynchronize(ev->stop_event_dev);
			devcall_assert(result);
			result = cudaEventElapsedTime(&elapse, ev->start_event_dev, ev->stop_event_dev);
			devcall_assert(result);
		} else
#endif
#endif
		if (devtype == OMP_DEVICE_THSIM) {
			elapse = ev->stop_time_dev - ev->start_time_dev;
		} else {
			fprintf(stderr, "other type of devices are not yet supported\n");
		}
		ev->elapsed_dev = elapse;
	}

	if (record_method == OMP_EVENT_HOST_RECORD || record_method == OMP_EVENT_HOST_DEV_RECORD) {
		ev->elapsed_host = ev->stop_time_host - ev->start_time_host;
	}
}

/**
 * sync device by syncing the stream so all the pending calls the stream are completed
 *
 * if destroy_stream != 0; the stream will be destroyed.
 */
void omp_stream_sync(omp_dev_stream_t *st, int destroy_stream) {
#if defined (DEVICE_NVGPU_SUPPORT)
	cudaError_t result;
	if (destroy_stream) {
		result = cudaStreamSynchronize(st->systream.cudaStream);
		devcall_assert(result);
		result = cudaStreamDestroy(st->systream.cudaStream);
		devcall_assert(result);
	} else {
		result = cudaStreamSynchronize(st->systream.cudaStream);
		devcall_assert(result);
	}
#else
#endif
}

/**
 * seqid is the sequence id of the device in the top, it is also used as index to access maps
 *
 */
void omp_sync_cleanup(omp_offloading_t * off) {
	int i;
	omp_offloading_info_t * off_info = off->off_info;
	omp_stream_sync(&off->stream, 1);

	for (i = 0; i < off_info->num_mapped_vars; i++) {
		omp_data_map_t * map = &off_info->data_map_info[i].maps[off->devseqid];
		omp_map_free_dev(map->dev, map->map_dev_ptr);
		if (map->marshalled_or_not) { /* if this is marshalled and need to free space since this is not useful anymore */
			omp_map_unmarshal(map);
			free(map->map_buffer);
		}
	}
}


