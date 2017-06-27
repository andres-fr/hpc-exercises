/*
This program will numerically compute the integral of

                  4/(1+x*x) 
          
from 0 to 1.  The value of this integral is pi -- which 
is great since it gives us an easy way to check the answer.

The is the original sequential program.  It uses the timer
from the OpenMP runtime library

History: Written by Tim Mattson, 11/99.

*/

// TODO: 1. parallelize with OpenMP using only #pragma omp parallel
//       2. parallelize with OpenMP making as small changes in the program as possible

#include <stdio.h>
#include <omp.h>

static long num_steps = 100000000;
double step;
int main ()
{
  int i;
  double x, pi, sum = 0.0;
  double start_time, run_time, run_time2, run_time3;
  int NThreads = 50;
  step = 1.0/(double) num_steps;

  // 1) SCALAR SOLUTION
  start_time = omp_get_wtime();
  for (i=1;i<= num_steps; i++){
    x = i*step; // (i-0.5)*step // ORIGINAL -0.5, WHY?
    sum = sum + 4.0/(1.0+x*x);
  }
  pi = step * sum;
  run_time = omp_get_wtime() - start_time;
  printf("\npi with %ld steps is %f in %f seconds \n",num_steps,pi,run_time);


  // implementation with pragma omp parallel
  sum = 0;
  start_time = omp_get_wtime();
  #pragma omp parallel for num_threads(NThreads)  reduction(+:sum)
  for(i=0; i<num_steps; ++i){
    x = i*step; // (i-0.5)*step // ORIGINAL -0.5, WHY?
    sum += 4.0/(1.0+x*x);
  }
  pi = step * sum;
  run_time2 = omp_get_wtime() - start_time;
  printf("\nPARALLELIZED pi with %d threads and %ld steps is %f in %f seconds \n",
         NThreads, num_steps,pi,run_time2);
  printf("\nspeedup: %f\n", run_time/run_time2);


  // 1) OPENMP SMALL CHANGES
  start_time = omp_get_wtime();
#pragma omp parallel for num_threads(NThreads)  reduction(+:sum) private(i,x) // why is private here useful?
  for (i=1;i<= num_steps; i++){
    x = i*step; // (i-0.5)*step // ORIGINAL -0.5, WHY?
    sum = sum + 4.0/(1.0+x*x);
  }
  pi = step * sum;
  run_time3 = omp_get_wtime() - start_time;
  printf("\nPARALLELIZED v2 pi with %d threads and %ld steps is %f in %f seconds \n",
         NThreads, num_steps,pi,run_time3);
  printf("\nspeedup: %f\n", run_time/run_time3);




}
