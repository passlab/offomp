#ifndef __AXPY_H__
#define __AXPY_H__

/* change this to do saxpy or daxpy : single precision or double precision*/
#define REAL double

#ifdef __cplusplus
extern "C" {
#endif

/* both the omp version and ompacc version */
extern void axpy_omp(REAL* x, REAL* y, int n, REAL a); 
extern void axpy_ompacc(REAL* x, REAL* y, int n, REAL a); 
extern double read_timer(); /* in second */
extern double read_timer_ms(); /* in ms */
#ifdef __cplusplus
 }
#endif

#endif
