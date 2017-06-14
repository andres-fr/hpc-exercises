/*
This program will numerically compute the integral of

                  4/(1+x*x) 
  
from 0 to 1.  The value of this integral is pi -- which 
is great since it gives us an easy way to check the answer.

The is the original sequential program.  It uses the timer
from the OpenMP runtime library

History: Written by Tim Mattson, 11/99.

*/
#include <stdio.h>
#include <omp.h>

static long num_steps = 100000000;
double step;
int main ()
{
  
  double pi, sum = 0.0;
  double start_time, run_time;

  step = 1.0/(double) num_steps;
         
  start_time = omp_get_wtime();

  const int nThreads = 2;
  double sums[nThreads] = {0};
  omp_set_num_threads(nThreads);
  #pragma omp parallel
  {
    int id = omp_get_thread_num();
     
    for (int i=1;i<= num_steps/nThreads; i++){
      double x = (i + id* num_steps/nThreads -0.5)*step;
      sums[id] = sums[id] + 4.0/(1.0+x*x);
    }
  }

  for (int i = 0; i < nThreads; i++ )
    sum += sums[i];
  
  pi = step * sum;
  run_time = omp_get_wtime() - start_time;
  printf("\n pi with %d steps is %f in %f seconds \n",num_steps,pi,run_time);
}  





