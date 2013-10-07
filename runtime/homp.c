/*
 * homp.c
 *
 *  Created on: Sep 16, 2013
 *      Author: yy8
 */

#include "homp.h"

/* OpenMP 4.0 support */
int default_device_var = -1;
void omp_set_default_device(int device_num ) {
  default_device_var = device_num;
}
int omp_get_default_device(void) {
  return default_device_var;
}

int omp_get_num_devices() {
	return omp_num_devices;
}

omp_device_t * omp_devices;
int omp_num_devices;
char * omp_device_type_name[OMP_NUM_DEVICE_TYPES];

/* APIs to support multiple devices: */
char * omp_supported_device_types() { /* return a list of devices supported by the compiler in the format of TYPE1:TYPE2 */
	/* FIXME */
	return "OMP_DEVICE_NVGPU";
}
omp_device_type_t omp_get_device_type(int devid) {
	return omp_devices[devid].type;
}

char * omp_get_device_type_as_string(int devid) {
	/* FIXME */
	return "OMP_DEVICE_NVGPU";
}

int omp_get_num_devices_of_type(omp_device_type_t type) { /* current omp has omp_get_num_devices(); */
	int num = 0;
	int i;
	for (i=0; i<omp_num_devices; i++)
		if (omp_devices[i].type == type) num++;
	return num;
}
/*
 * return the first ndev device IDs of the specified type, the function returns the actual number of devices
 * in the array (devnum_array)
 *
 * before calling this function, the caller should allocate the devnum_array[ndev]
 */
int omp_get_devices(omp_device_type_t type, int *devnum_array, int ndev) { /* return a list of devices of the specified type */
	int i;
	int num = 0;
	for (i=0; i<omp_num_devices; i++)
		if (omp_devices[i].type == type && num <= ndev) {
			devnum_array[num] = omp_devices[i].id;
			num ++;
		}
	return num;
}
omp_device_t * omp_get_device(int id) {
	return &omp_devices[id];
}

static void omp_query_device_count(int * count) {
	/* currently, only do the CUDA GPUs */
	cudaGetDeviceCount(count);
}

/* init the device objects, num_of_devices, default_device_var ICV etc */
void omp_init_devices() {
	omp_query_device_count(&omp_num_devices);
	omp_devices = malloc(sizeof(omp_device_t) * omp_num_devices);
	int i;
	for (i=0; i<omp_num_devices; i++)
	{
		omp_devices[i].id = i;
		omp_devices[i].type = OMP_DEVICE_NVGPU;
		omp_devices[i].status = 1;
		omp_devices[i].next = &omp_devices[i+1];
	}
	if (omp_num_devices) {
		default_device_var = 0;
		omp_devices[omp_num_devices-1].next = NULL;
	}
}

void omp_set_current_device(omp_device_t * d) {
	if (d->type == OMP_DEVICE_NVGPU) {
		cudaSetDevice(d->sysid);
	} else {
		fprintf(stderr, "device type (%d) is not yet supported!\n", d->type);
	}
}

void omp_init_stream(omp_device_t * d, omp_stream_t * stream) {
	stream->dev = d;
	if (d->type == OMP_DEVICE_NVGPU) {
		cudaStreamCreate(&stream->systream.cudaStream);
	} else {
		fprintf(stderr, "device type (%d) is not yet supported!\n", d->type);
	}
}
void omp_data_map_init_source(omp_data_map_t *map, void * source_ptr, int sizeof_element, long dim0, long dim1, long dim2) {
	map->source_ptr = source_ptr;
	map->dim[0] = dim0;
	map->dim[1] = dim1;
	map->dim[2] = dim2;
	map->sizeof_element = sizeof_element;
}
void omp_data_map_init_map(omp_data_map_t *map, int local_devid, omp_grid_topology_t * top,
		omp_device_t * dev, omp_map_type_t type, omp_stream_t * stream) {
	map->dev = dev;
	map->map_type = type;
	map->stream = stream;
	map->local_devid = local_devid;
	map->top = top;
}

/**
 * given a sqeuence id, return the top coordinates
 * the function return the actual number of dimensions
 */
int omp_get_coords_topology(omp_grid_topology_t * top, int sid, int ndims, int coords[]) {
	if (top->ndims > ndims) {
		sprintf(stderr, "the given ndims and array are too small\n");
		return -1;
	}
	int i, nnodes;
	nnodes = top->nnodes;
    for ( i=0; i < top->ndims; i++ ) {
    	nnodes    = nnodes / top->dims[i];
    	coords[i] = sid / nnodes;
        sid  = sid % nnodes;
    }
    return i;
}

/**
 * for a given device (with sequence id devsid in the target device list) who is part of the topology (top) of topdim dimension, apply
 * the even distribution of the dim of the source array in map
 */
void omp_data_map_do_even_map(omp_data_map_t *map, int dim, omp_grid_topology_t *top, int topdim, int devsid) {
	/* calculate the coordinates of id in the top */
	int coords[top->ndims];
	omp_get_coords_topology(top, devsid, top->ndims, coords);

    int dimcoord = coords[topdim];
    int dimsize = top->dims[dim];

    /* partition the array region into subregion and save it to the map */
    int n = map->dim[dim];
    int remaint = n % dimsize;
    int esize = n / dimsize;
    if (dimcoord < remaint) { /* each of the first remaint dev has one more element */
    	map->map_dim[dim] = esize+1;
        map->map_offset[dim] = (esize+1)*dimcoord;
    } else {
    	map->map_dim[dim] = esize;
        map->map_offset[dim] = esize*dimcoord + remaint;
    }
}

void omp_data_map_do_fix_map(omp_data_map_t * map, int dim, int start, int length, int devsid) {
	map->map_dim[dim] = length;
	map->map_offset[dim] = start;
}

int linearize2D(int X, int Y, int i, int j) {
	return i*Y+j;
}

void cartize2D(int X, int Y, int num, int *i, int *j) {
	*i = num/Y;
	*j = num%Y;
}

int linearize3D(int X, int Y, int Z, int i, int j, int k) {
	return 0;
}

void cartize3D(int X, int Y, int Z, int num, int *i, int *j, int *k) {
}

void map3D(int X, int Y, int Z, int startX, int subX, int startY, int subY, int startZ, int subZ, int i, int j, int k, int *subi, int *subj, int *subk) {
}

void rev_map3D(int X, int Y, int Z, int startX, int subX, int startY, int subY, int startZ, int subZ, int *i, int *j, int *k, int subi, int subj, int subk) {
}

void omp_marshalArrayRegion(omp_data_map_t * dmap) {
}

void omp_unmarshalArrayRegion(omp_data_map_t * dmap) {
}

void omp_print_data_map(omp_data_map_t * map) {
	printf("MAP: %X, source ptr: %X, dim[0]: %ld, dim[1]: %ld, dim[2]: %ld, map_dim[0]: %ld, map_dim[1]: %ld, map_dim[2]: %ld, "
				"map_offset[0]: %ld, map_offset[1]: %ld, map_offset[2]: %ld, sizeof_element: %d, map_buffer: %X, marshall_or_not: %d,"
				"map_dev_ptr: %X, stream: %X, mem_size: %ld, device_id: %d\n\n", map, map->source_ptr, map->dim[0], map->dim[1], map->dim[2],
				map->map_dim[0], map->map_dim[1], map->map_dim[2], map->map_offset[0], map->map_offset[1], map->map_offset[2],
				map->sizeof_element, map->map_buffer, map->marshalled_or_not, map->map_dev_ptr, map->stream, map->mem_size, map->device_id);
}

void omp_map_init_add_halo_region(omp_data_map_t * map, int dim, int left, int right, int cyclic) {
	int i;
	for (i=0; i<OMP_NUM_ARRAY_DIMENSIONS; i++)
		map->halo_region[i].left = map->halo_region[i].right = map->halo_region[i].cyclic = 0;

	map->halo_region[dim].left = left;
	map->halo_region[dim].right = right;
	map->halo_region[dim].cyclic = cyclic;
}

void omp_map_add_halo_region(omp_data_map_t * map, int dim, int left, int right, int cyclic) {
	map->halo_region[dim].left = left;
	map->halo_region[dim].right = right;
	map->halo_region[dim].cyclic = cyclic;
}

/* do a halo regin pull for data map of devid. If top is not NULL, devid will be translated to coordinate of the
 * virtual topology and the halo region pull will be based on this coordinate. the int dim[] array specify which dimensions
 * to do the halo region update. If dim == NULL, do all the update of dimensions that has halo region
 *
 */
void omp_halo_region_pull(int devid, omp_grid_topology_t * top, omp_data_map_t * map, int dim[]) {
	int i;
	if (top == NULL && dim == NULL) {
		for (i=0; i<OMP_NUM_ARRAY_DIMENSIONS; i++) {

		}
	}


}

void omp_halo_region_pull_async(int devid, omp_grid_topology_t * top, omp_data_map_t * map, int dim[]) {
}
/**
 * this function creates host buffer, if needed, and marshall data to the host buffer,
 *
 * it will also create device memory region (both the array region memory and halo region memory
 */
void omp_map_buffer(omp_data_map_t * map, int marshal, short halo_region_or_not) {
	map->marshalled_or_not = marshal;
	map->halo_region_or_not = halo_region_or_not;

	long map_size = map->sizeof_element;
	int i;
	for (i=0; i<OMP_NUM_ARRAY_DIMENSIONS; i++) {
		map_size *= map->map_dim[i];
	}
	map->map_size = map_size;
	if (!marshal) map->map_buffer = map->source_ptr + map->map_offset[0]*map->sizeof_element; /* TODO: if it is 1-dimension, or two-dimension with contigunous memory, etc */
	else omp_marshalArrayRegion(map);

	/* we need to allocate device memory, including both the array region and halo region */

	if (!halo_region_or_not) {
		map->mem_size = map_size;
		cudaMalloc(&map->mem_dev_ptr, map_size);
		map->map_dev_ptr = map->mem_dev_ptr;
	} else { /* halo region */
		long mem_size = map->sizeof_element;
		for (i=0; i<OMP_NUM_ARRAY_DIMENSIONS; i++) {
			mem_size *= (map->map_dim[i]+map->halo_region[i][0]+map->halo_region[i][1]);
		}
		map->mem_size = mem_size;
		cudaMalloc(&map->mem_dev_ptr, map_size);
		map->map_dev_ptr = map->mem_dev_ptr + map->halo_region[0][0]*map->sizeof_element; /* TODO: only 1-D cases now */
	}
}

/**
 * return the mapped range index from the iteration range of the original array
 * e.g. A[128], when being mapped to a device for A[64:64] (from 64 to 128), then, the range 100 to 128 in the original A will be
 * mapped to 36 to 64 in the mapped region of A
 *
 * @param: omp_data_map_t * map: the mapped variable, we should use the original pointer and let the runtime retrieve the map
 * @param: int dim: which dimension to retrieve the range
 * @param: int start: the start index from the original array, if start is -1, use the map_offset_<dim>, which will simply cause
 * 					the function return 0 for obvious reasons
 * @param: int length: the length of the range, if -1, use the mapped dim from the start
 * @param: int * map_start: the mapped start index in the mapped range, if return <0 value, wrong input
 * @param: int * map_length: normally just the length, if lenght == -1, use the map_dim[dim]
 *
 * NOTE: the mapped range must be a subset of the range of the specified map in the specified dim
 *
 */
void omp_loop_map_range (omp_data_map_t * map, int dim, long start, long length, long * map_start, long * map_length) {
	if (start <=0) {
		if (length < 0) {
			*map_start = 0;
			*map_length = map->map_dim[dim];
			return;
		} else if (length <= map->map_dim[dim]) {
			*map_start = 0;
			*map_length = length;
			return;
		} else {
			/* error */
		}
	} else { /* start > 0 */
		*map_start = start - map->map_offset[dim];
		*map_length = map->map_dim[dim] - *map_start; /* the max length */
		if (*map_start < 0) { /* out of the range */
			*map_length = -1;
			return;
		} else if (length <= *map_length) {
			*map_length = length;
			return;
		}
	}

	/* out of range */
	*map_start = -1;
	*map_length = -1;
	return;
}

/*
 * marshalled the array region of the source array, and copy data to to its new location (map_buffer)
 */
void omp_memcpyHostToDeviceAsync(omp_data_map_t * map) {
	cudaMalloc(&map->map_dev_ptr, map->mem_size);
	cudaMemcpyAsync((void *)map->map_dev_ptr,(const void *)map->map_buffer,map->mem_size, cudaMemcpyHostToDevice, *map->stream);
}

void omp_memcpyDeviceToHostAsync(omp_data_map_t * map) {
    cudaMemcpyAsync((void *)map->map_buffer,(const void *)map->map_dev_ptr,map->mem_size, cudaMemcpyDeviceToHost, *map->stream);
}

void omp_memcpyDeviceToHost(omp_data_map_t * map) {
    cudaMemcpy((void *)map->map_buffer,(const void *)map->map_dev_ptr,map->mem_size, cudaMemcpyDeviceToHost);
}

/**
 * sync device by syncing the stream so all the pending calls the stream are completed
 *
 * if destroy_stream != 0; the stream will be destroyed.
 */
void omp_sync_stream(int num_devices, cudaStream_t dev_stream[num_devices], int destroy_stream) {
	int i;

	if (destroy_stream){
		for (i=0; i<num_devices; i++) {
			cudaSetDevice(i);
			//Wait for all operations to finish
			cudaStreamSynchronize(dev_stream[i]);
			cudaStreamDestroy(dev_stream[i]);
		}
	} else {
		for (i=0; i<num_devices; i++) {
			cudaSetDevice(i);
			//Wait for all operations to finish
			cudaStreamSynchronize(dev_stream[i]);
		}
	}
}

/*
 * When call this function, the stream should already synced
 */
void omp_map_device2host(int num_devices, int num_maps, omp_data_map_t *data_map) {
	int i, j;

	for (i=0; i<num_devices; i++) {
		cudaSetDevice(i);
	    //Wait for all operations to finish
	    for (j=0; j<num_maps; j++) {
	    	omp_data_map_t * map = &data_map[i*num_maps+j];
//	    	printf("postACCKernel map: %X\n", map);
	    	omp_unmarshalArrayRegion(map);
	    	cudaFree(map->map_dev_ptr);
	    	if (map->marshalled_or_not) { /* if this is marshalled and need to free space since this is not useful anymore */
	    		free(map->map_buffer);
	    	}
	    }
	}
}

void omp_postACCKernel(int num_devices, int num_maps, cudaStream_t dev_stream[num_devices], omp_data_map_t *data_map) {
	int i, j;

	for (i=0; i<num_devices; i++) {
		cudaSetDevice(i);
	    //Wait for all operations to finish
	    cudaStreamSynchronize(dev_stream[i]);
	    for (j=0; j<num_maps; j++) {
	    	omp_data_map_t * map = &data_map[i*num_maps+j];
//	    	printf("postACCKernel map: %X\n", map);
	    	omp_unmarshalArrayRegion(map);
	    	cudaFree(map->map_dev_ptr);
	    	if (map->marshalled_or_not) { /* if this is marshalled and need to free space since this is not useful anymore */
	    		free(map->map_buffer);
	    	}
	    }
	    cudaStreamDestroy(dev_stream[i]);
	}
}

size_t xomp_get_maxThreadsPerBlock()
{
  // this often causes oversubscription to the cores supported by GPU SM processors
  //return xomp_getCudaDeviceProp()->maxThreadsPerBlock;
  return 128;
}

size_t xomp_get_max1DBlock(size_t s)
{
  size_t block_num = s/xomp_get_maxThreadsPerBlock();
  if (s % xomp_get_maxThreadsPerBlock()!= 0)
     block_num ++;
  return block_num;
}

/**
 * utilities
 */

/**
 * factor n into dims number of numbers whose multiplication equals to n
 */
void omp_factor(int n, int factor[], int dims) {
	switch (dims) {
	case 1:
	{
		factor[0] = n;
		return;
	}
	case 2:
	{
		switch (n) {
		case 1:
		case 2:
		case 3:
		case 5:
		case 7:
		case 11:
		case 13:
		{
			factor[0] = n;
			factor[1] = 1;
			return;
		}
		case 4:
		case 6:
		case 8:
		case 10:
		case 14:
		{
			factor[0] = n/2;
			factor[1] = 2;
			return;
		}
		case 9:
		case 15:
		{
			factor[0] = n/3;
			factor[1] = 3;
			return;
		}
		case 12:
		case 16:
		{
			factor[0] = n/4;
			factor[1] = 4;
			return;
		}
		}
		break;
	}
	case 3:
	{
		switch (n) {
		case 1:
		case 2:
		case 3:
		case 5:
		case 7:
		case 11:
		case 13:
		{
			factor[0] = n;
			factor[1] = 1;
			factor[2] = 1;
			return;
		}
		case 4:
		case 6:
		{
			factor[0] = n/2;
			factor[1] = 2;
			factor[2] = 1;
			return;
		}
		case 8:
		{
			factor[0] = 2;
			factor[1] = 2;
			factor[2] = 2;
			return;
		}
		default: break;
		}
	}
	default:
		fprintf(stderr, 'more than 3 dimensions are not supported\n');
	}
}


