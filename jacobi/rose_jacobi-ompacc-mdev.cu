#include <stdio.h>
#include <math.h>
#ifdef _OPENMP
#include <omp.h>
#endif
// Add timing support
#include <sys/time.h>
#include "libxomp.h" 
#include "xomp_cuda_lib_inlined.cu" 
#include "homp.h"

double time_stamp() {
	struct timeval t;
	double time;
	gettimeofday(&t, 0);
	time = (t.tv_sec + (1.0e-6 * t.tv_usec));
	return time;
}
double time1;
double time2;
void driver();
void initialize();
void jacobi();
void error_check();
/************************************************************
 * program to solve a finite difference
 * discretization of Helmholtz equation :
 * (d2/dx2)u + (d2/dy2)u - alpha u = f
 * using Jacobi iterative method.
 *
 * Modified: Sanjiv Shah,       Kuck and Associates, Inc. (KAI), 1998
 * Author:   Joseph Robicheaux, Kuck and Associates, Inc. (KAI), 1998
 *
 * This c version program is translated by
 * Chunhua Liao, University of Houston, Jan, 2005
 *
 * Directives are used in this code to achieve parallelism.
 * All do loops are parallelized with default 'static' scheduling.
 *
 * Input :  n - grid dimension in x direction
 *          m - grid dimension in y direction
 *          alpha - Helmholtz constant (always greater than 0.0)
 *          tol   - error tolerance for iterative solver
 *          relax - Successice over relaxation parameter
 *          mits  - Maximum iterations for iterative solver
 *
 * On output
 *       : u(n,m) - Dependent variable (solutions)
 *       : f(n,m) - Right hand side function
 *************************************************************/
#ifndef MSIZE
#warning "MSIZE default to be 512"
#define MSIZE 512
#endif

int n;
int m;
int mits;
#define REAL float // flexible between float and double
float tol;
float relax = 1.0;
float alpha = 0.0543;
float u[MSIZE][MSIZE];
float f[MSIZE][MSIZE];
float uold[MSIZE][MSIZE];
float dx;
float dy;

int main() {
//  float toler;
	/*      printf("Input n,m (< %d) - grid dimension in x,y direction:\n",MSIZE);
	 scanf ("%d",&n);
	 scanf ("%d",&m);
	 printf("Input tol - error tolerance for iterative solver\n");
	 scanf("%f",&toler);
	 tol=(double)toler;
	 printf("Input mits - Maximum iterations for solver\n");
	 scanf("%d",&mits);
	 */
	omp_init_devices();
	n = MSIZE;
	m = MSIZE;
	printf("Input n,m (< %d) - grid dimension in x,y direction.\n", MSIZE);
	tol = 0.0000000001;
	mits = 5000;
#if 0 // Not yet support concurrent CPU and GPU threads  
#ifdef _OPENMP
#endif
#endif  
	driver();
	return 0;
}
/*************************************************************
 * Subroutine driver ()
 * This is where the arrays are allocated and initialzed.
 *
 * Working varaibles/arrays
 *     dx  - grid spacing in x direction
 *     dy  - grid spacing in y direction
 *************************************************************/

void driver() {
	initialize();
	time1 = time_stamp();
	/* Solve Helmholtz equation */
	jacobi();
	time2 = time_stamp();
	printf("------------------------\n");
	printf("Execution time = %f\n", (time2 - time1));
	/* error_check (n,m,alpha,dx,dy,u,f)*/
	error_check();
}
/*      subroutine initialize (n,m,alpha,dx,dy,u,f) 
 ******************************************************
 * Initializes data
 * Assumes exact solution is u(x,y) = (1-x^2)*(1-y^2)
 *
 ******************************************************/

void initialize() {
	int i;
	int j;
	int xx;
	int yy;
//double PI=3.1415926;
	dx = (2.0 / (n - 1));
	dy = (2.0 / (m - 1));
	/* Initialize initial condition and RHS */
//#pragma omp parallel for private(xx,yy,j,i)
	for (i = 0; i < n; i++)
		for (j = 0; j < m; j++) {
			xx = ((int) (-1.0 + (dx * (i - 1))));
			yy = ((int) (-1.0 + (dy * (j - 1))));
			u[i][j] = 0.0;
			f[i][j] = (((((-1.0 * alpha) * (1.0 - (xx * xx)))
					* (1.0 - (yy * yy))) - (2.0 * (1.0 - (xx * xx))))
					- (2.0 * (1.0 - (yy * yy))));
		}
}
/*      subroutine jacobi (n,m,dx,dy,alpha,omega,u,f,tol,maxit)
 ******************************************************************
 * Subroutine HelmholtzJ
 * Solves poisson equation on rectangular grid assuming :
 * (1) Uniform discretization in each direction, and
 * (2) Dirichlect boundary conditions
 *
 * Jacobi method is used in this routine
 *
 * Input : n,m   Number of grid points in the X/Y directions
 *         dx,dy Grid spacing in the X/Y directions
 *         alpha Helmholtz eqn. coefficient
 *         omega Relaxation factor
 *         f(n,m) Right hand side function
 *         u(n,m) Dependent variable/Solution
 *         tol    Tolerance for iterative solver
 *         maxit  Maximum number of iterations
 *
 * Output : u(n,m) - Solution
 *****************************************************************/
#if 1 
__global__ void OUT__1__10550__(int n, int m, float omega, float ax, float ay,
		float b, float *_dev_per_block_error, float *_dev_u, float *_dev_f,
		float *_dev_uold) {
	int _dev_i;
	int _p_j;

	float _p_error;
	_p_error = 0;
	float _p_resid;

	long _dev_lower, _dev_upper;
	XOMP_accelerator_loop_default(1, n - 2, 1, &_dev_lower, &_dev_upper);

	for (_dev_i = _dev_lower; _dev_i <= _dev_upper; _dev_i++) {
		for (_p_j = 1; _p_j < (m - 1); _p_j++) {
			_p_resid = (((((ax
					* (_dev_uold[(_dev_i - 1) * MSIZE + _p_j]
							+ _dev_uold[(_dev_i + 1) * MSIZE + _p_j]))
					+ (ay
							* (_dev_uold[_dev_i * MSIZE + (_p_j - 1)]
									+ _dev_uold[_dev_i * MSIZE + (_p_j + 1)])))
					+ (b * _dev_uold[_dev_i * MSIZE + _p_j]))
					- _dev_f[_dev_i * MSIZE + _p_j]) / b);
			_dev_u[_dev_i * MSIZE + _p_j] = (_dev_uold[_dev_i * MSIZE + _p_j]
					- (omega * _p_resid));
			_p_error = (_p_error + (_p_resid * _p_resid));
		}
	}
	xomp_inner_block_reduction_float(_p_error, _dev_per_block_error, 6);
}

#else
__global__ void OUT__1__10550__(int n,int m,float omega,float ax,float ay,float b,float *_dev_per_block_error,float *_dev_u,float *_dev_f,float *_dev_uold)
{
	int _p_i;
	int _p_j;
	float _p_error;
	_p_error = 0;
	float _p_resid;
	int _dev_i = blockDim.x * blockIdx.x + threadIdx.x;
	if (_dev_i >= 1 && _dev_i <= (n - 1) - 1) {
		for (_p_j = 1; _p_j < (m - 1); _p_j++) {
			_p_resid = (((((ax * (_dev_uold[(_dev_i - 1) * MSIZE + _p_j] + _dev_uold[(_dev_i + 1) * MSIZE + _p_j])) + (ay * (_dev_uold[_dev_i * MSIZE + (_p_j - 1)] + _dev_uold[_dev_i * MSIZE + (_p_j + 1)]))) + (b * _dev_uold[_dev_i * MSIZE + _p_j])) - _dev_f[_dev_i * MSIZE + _p_j]) / b);
			_dev_u[_dev_i * MSIZE + _p_j] = (_dev_uold[_dev_i * MSIZE + _p_j] - (omega * _p_resid));
			_p_error = (_p_error + (_p_resid * _p_resid));
		}
	}
	xomp_inner_block_reduction_float(_p_error,_dev_per_block_error,6);
}
#endif

#if 1
__global__ void OUT__2__10550__(int n, int m, float *_dev_u, float *_dev_uold) {
	int _p_j;
	int _dev_i;
	long _dev_lower, _dev_upper;
	XOMP_accelerator_loop_default(0, n - 1, 1, &_dev_lower, &_dev_upper);
	for (_dev_i = _dev_lower; _dev_i <= _dev_upper; _dev_i++) {
		for (_p_j = 0; _p_j < m; _p_j++)
			_dev_uold[_dev_i * MSIZE + _p_j] = _dev_u[_dev_i * MSIZE + _p_j];
	}
}

#else
__global__ void OUT__2__10550__(int n,int m,float *_dev_u,float *_dev_uold)

{
	int _p_i;
	int _p_j;
	int _dev_i = blockDim.x * blockIdx.x + threadIdx.x;
	if (_dev_i >= 0 && _dev_i <= n - 1) {
		for (_p_j = 0; _p_j < m; _p_j++)
		_dev_uold[_dev_i * MSIZE + _p_j] = _dev_u[_dev_i * MSIZE + _p_j];
	}
}

#endif 
void jacobi() {
	float omega;
	int i;
	int j;
	int k;
	float error;
	float resid;
	float ax;
	float ay;
	float b;
	omega = relax;
	/*
	 * Initialize coefficients */
	/* X-direction coef */
	ax = (1.0 / (dx * dx));
	/* Y-direction coef */
	ay = (1.0 / (dy * dy));
	/* Central coeff */
	b = (((-2.0 / (dx * dx)) - (2.0 / (dy * dy))) - alpha);
	error = (10.0 * tol);
	k = 1;
	/*
	 #pragma omp target data device(*)=>(*)(*) map(to:n, m, omega, ax, ay, b, f[0:n][0:m]>>(:)(:)) map(tofrom:u[0:n][0:m]>>(:)(:)) map(alloc:uold[0:n|1][0:m]>>(:)(:))
	 */
	/* there are three mapped array variables (f, u, and uold). all scalar variables will be as parameters */
	int __num_target_devices__ = 4; /*XXX: = runtime call or compiler generated number */
	omp_device_t *__target_devices__[__num_target_devices__];
	/**TODO: compiler generated code or runtime call to init the __target_devices__ array */
	int __ndev_i__;
	for (__ndev_i__ = 0; __ndev_i__ < __num_target_devices__; __ndev_i__++) {
		__target_devices__[__ndev_i__] = &omp_devices[__ndev_i__]; /* currently this is simple a copy of the pointer */
	}
	/**TODO: compiler generated code or runtime call to init the topology */
	int __top_ndims__ = 2;
	int __top_dims__[__top_ndims__];
	omp_factor(__num_target_devices__, __top_dims__, __top_ndims__);
	int __top_periodic__[__top_ndims__]; __top_periodic__[0] = 0;__top_periodic__[1] = 0; /* this is not used at all */
	omp_grid_topology_t __topology__={__num_target_devices__, __top_ndims__, __top_dims__, NULL};
	omp_grid_topology_t *__topp__ = &__topology__;

	int __num_mapped_variables__ = 3; /* XXX: need compiler output */

	omp_stream_t __dev_stream__[__num_target_devices__]; /* need to change later one for omp_stream_t struct */
	omp_data_map_t __data_maps__[__num_target_devices__][__num_mapped_variables__];
	for (__ndev_i__ = 0; __ndev_i__ < __num_target_devices__; __ndev_i__++) {
		omp_device_t * __dev__ = __target_devices__[__ndev_i__];
		omp_set_current_device(__current_dev__);
		omp_init_stream(__current_dev__, &__dev_stream__[__ndev_i__]);

		/***************** for each mapped variable has to and tofrom, if it has region mapped to this __ndev_i__ id, we need code here *******************************/
		omp_data_map_t * __dev_map_f__ = &__data_maps__[__ndev_i__][0]; /* 0 is given by compiler here */
		omp_data_map_init_source(__dev_map_f__, &f[0][0], sizeof(float), n, m, 1);
		omp_data_map_init_map(__dev_map_f__, __ndev_i__, __dev__, __topp__, OMP_MAP_TO, &__dev_stream__[__ndev_i__]);
		omp_data_map_do_even_map(__dev_map_f__, 0, __topp__, 0, __ndev_i__);
		omp_data_map_do_even_map(__dev_map_f__, 1, __topp__, 1, __ndev_i__);

		omp_map_buffer(__dev_map_f__, 0); /* even a 2-d array, but since we are doing row-major partition, no need to marshalled data */

		omp_memcpyHostToDeviceAsync(__dev_map_f__);
		omp_print_data_map(__dev_map_f__);
		/*************************************************************************************************************************************************************/

		/***************************************************************** for u *********************************************************************/
		omp_data_map_t * __dev_map_u__ = &__data_maps__[__ndev_i__][1]; /* 1 is given by compiler here */
		omp_data_map_init_source(__dev_map_u__, &u[0][0], sizeof(float), n, m, 1);
		omp_data_map_init_map(__dev_map_u__, __ndev_i__, __dev__, __topp__, OMP_MAP_TOFROM, &__dev_stream__[__ndev_i__]);

		omp_data_map_do_even_map(__dev_map_u__, 0, __topp__, 0, __ndev_i__);
		omp_data_map_do_even_map(__dev_map_u__, 1, __topp__, 1, __ndev_i__);

		omp_map_buffer(__dev_map_u__, 1); /* column major, marshalling needed */

		omp_memcpyHostToDeviceAsync(__dev_map_u__);
		omp_print_data_map(__dev_map_u__);

		/******************************************** for uold ******************************************************************************/

		omp_data_map_t * __dev_map_uold__ = &__data_maps__[__ndev_i__][2]; /* 2 is given by compiler here */
		omp_data_map_init_source(__dev_map_uld__, &uold[0][0] /* NULL */, sizeof(float), n, m, 1);
		omp_data_map_init_map(__dev_map_uold__, __ndev_i__, __dev__, __topp__, OMP_MAP_ALLOC, &__dev_stream__[__ndev_i__]);

		omp_data_map_do_even_map(__dev_map_uold__, 0, __topp__, 0, __ndev_i__);
		omp_data_map_do_even_map(__dev_map_uold__, 1, __topp__, 1, __ndev_i__);
		/* handle halo region here */
		omp_map_init_add_halo_region(__dev_map_uold__, 0, 1, 1, 0);
		omp_map_init_add_halo_region(__dev_map_uold__, 1, 1, 1, 0);

		omp_map_buffer(__dev_map_uold__, 0);

		omp_print_data_map(__dev_map_uold__);
	}

	while ((k <= mits) && (error > tol)) {
		error = 0.0;
		/* Copy new solution into old */
		/* Launch CUDA kernel ... */
		for (__ndev_i__ = 0; __ndev_i__ < __num_target_devices__;__ndev_i__++) {
			cudaSetDevice(__ndev_i__);
			omp_data_map_t * __dev_map_f__ = &__data_maps__[__ndev_i__][0];
			omp_data_map_t * __dev_map_u__ = &__data_maps__[__ndev_i__][1];
			omp_data_map_t * __dev_map_uold__ = &__data_maps__[__ndev_i__][2]; /* 2 is given by compiler here */
			int _threads_per_block_ = xomp_get_maxThreadsPerBlock();
			int _num_blocks_ = xomp_get_max1DBlock(n / __num_target_devices__ - 1 - 0 + 1);
			OUT__2__10550__<<<_num_blocks_, _threads_per_block_, 0,
					__dev_stream__[__ndev_i__]>>>(n / __num_target_devices__, m,
					__dev_map_u__->map_dev_ptr, __dev_map_uold__->map_dev_ptr);

			/* halo exchange here, we do a pull protocol, thus the receiver move data from the source */
			omp_halo_region_pull_async(__ndev_i__, NULL, __dev_map_uold__,NULL);

			/* Launch CUDA kernel ... */
//			_threads_per_block_ = xomp_get_maxThreadsPerBlock();
			_num_blocks_ = xomp_get_max1DBlock((n / __num_target_devices__ - 1) - 1 - 1 + 1);
			float *_dev_per_block_error = (float *) (xomp_deviceMalloc(	_num_blocks_ * sizeof(float)));
			OUT__1__10550__<<<_num_blocks_, _threads_per_block_,(_threads_per_block_ * sizeof(float)),
					__dev_stream__[__ndev_i__]>>>(n / __num_target_devices__, m,
					omega, ax, ay, b, _dev_per_block_error,
					__dev_map_u__->map_dev_ptr, __dev_map_f__->map_dev_ptr,__dev_map_uold__->map_dev_ptr);
			/* copy back the results of reduction in blocks */
			float * _host_per_block_error = (float*)(malloc(_num_blocks_*sizeof(float)));
			cudaMemcpyAsync(_host_per_block_error, _dev_per_block_error, sizeof(float)*_num_blocks_, __dev_stream__[__ndev_i__]);
			omp_reduction_t beyond_block_reduction = {_host_per_block_error, _num_blocks_, sizeof(float), 6};
			cudaStreamAddCallback (__dev_stream__[__ndev_i__], xomp_beyond_block_reduction_float, &beyond_block_reduction, 0);
			/* error = xomp_beyond_block_reduction_float(_dev_per_block_error, _num_blocks_, 6); */
			//xomp_freeDevice(_dev_per_block_error);
		}
		/* here we sync the stream and make sure all are complete (including the per-device reduction)
		 */
		omp_sync_stream(__num_target_devices__, __dev_stream__, 0);
		/* then, we need the reduction from multi-devices */

		/* Error check */
		if ((k % 500) == 0)
			printf("Finished %d iteration with error =%f\n", k, error);
		error = (sqrt(error) / (n * m));
		k = (k + 1);
		/*  End iteration loop */
	}
	xomp_memcpyDeviceToHost(((void *) u), ((const void *) _dev_u), _dev_u_size);
	xomp_freeDevice (_dev_u);
	xomp_freeDevice (_dev_f);
	xomp_freeDevice (_dev_uold);
	printf("Total Number of Iterations:%d\n", k);
	printf("Residual:%E\n", error);
}
/*      subroutine error_check (n,m,alpha,dx,dy,u,f)
 implicit none
 ************************************************************
 * Checks error between numerical and exact solution
 *
 ************************************************************/

void error_check() {
	int i;
	int j;
	float xx;
	float yy;
	float temp;
	float error;
	dx = (2.0 / (n - 1));
	dy = (2.0 / (m - 1));
	error = 0.0;
//#pragma omp parallel for private(xx,yy,temp,j,i) reduction(+:error)
	for (i = 0; i < n; i++)
		for (j = 0; j < m; j++) {
			xx = (-1.0 + (dx * (i - 1)));
			yy = (-1.0 + (dy * (j - 1)));
			temp = (u[i][j] - ((1.0 - (xx * xx)) * (1.0 - (yy * yy))));
			error = (error + (temp * temp));
		}
	error = (sqrt(error) / (n * m));
	printf("Solution Error :%E \n", error);
}