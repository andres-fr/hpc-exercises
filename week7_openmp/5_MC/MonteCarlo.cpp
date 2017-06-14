  /// Monte Carlo pi estimation
  ///
  /// @authors: I.Kulakov; M.Zyzak; V.Akishina
  /// @e-mail I.Kulakov@gsi.de; M.Zyzak@gsi.de; V.Akishina@gsi.de
  /// 
  /// use "g++ MonteCarlo.cpp -O3 -fopenmp; ./a.out" to run

// Parallelize the SIMDized version between cores isng OpenMP. Compare the results and time.

using namespace std;
#include <stdio.h>
#include<iostream>
#include <stdlib.h>
#include "TStopwatch.h"

#include <omp.h>
#include <map>

static long num_trials = 10000000;

int main (int argc, char **argv)
{
    
    TStopwatch timerScalar;
    int i;  int Ncirc = 0;
    double pi, x, y, test;
    double r = 0.5;
    unsigned int seed = 0;
    
    int threads = atoi(argv[1]);
    int print = atoi(argv[2]);
   
    omp_set_num_threads(threads);
    
    map<int,int> threadNumberToCpuMap;
    
    for (int i=0; i<32; i++){
      threadNumberToCpuMap[i] = i;
    }
    
    #pragma omp parallel
    {
      int s;
      cpu_set_t cpuset;
      int cpuId = threadNumberToCpuMap[omp_get_thread_num()];
      pthread_t thread  = pthread_self();
      CPU_ZERO(&cpuset);
      CPU_SET(cpuId, &cpuset);
      s = pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);  
    }
      
    timerScalar.Start();

    for(i=0;i<num_trials; i++)
    {
        
       seed = i;
        
        x = ((double)rand_r(&seed)/RAND_MAX)-0.5;
    
        y = ((double)rand_r(&seed)/RAND_MAX)-0.5;
        
        test = x*x + y*y;
        
        if (test <= r*r) Ncirc++;
    }
    
    pi = 4.0 * ((double)Ncirc/(double)num_trials);
    
    timerScalar.Stop();
    double tScal = timerScalar.RealTime()*1000;
    
    if (print==1) {printf("\n %d trials, pi is %f \n",num_trials, pi);
        cout << "Time scalar: " << tScal << " ms " << endl;}
    
    if (print==0)  cout << tScal << endl;

    
    return 0;
}
