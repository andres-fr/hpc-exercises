/* ==================================================
   Exersice. Hello World. OpenMP
   Authors: I.Kulakov; M.Zyzak
   ================================================== */

// to compile and run the program type: g++ bug2.cpp -O3 -fopenmp -o bug2.out; ./bug2.out

// g++ -O0 -fno-tree-vectorize -fopenmp -std=c++14 -Wall -Wextra /home/afr/SS17/hpc-praktikum/week7_openmp/1_bugs/bug2.cpp -o bug2 && ./bug2

// Find a bug and correct it

#include "omp.h"
#include <stdlib.h> // rand
#include <cmath>
#include "../../libs/TStopwatch.h"

#include <iostream>
using namespace std;

static const int NThreads = 100;

int main() {

  int N = 10000;
  
  float *outputScalar=0, *outputParallel = 0;
  
  outputScalar = new float[N];
  outputParallel = new float[N];


  // SCALAR SOLUTION
  int tmp = 0;
  TStopwatch scalarTime;
  for(int i=1; i<N; i++)
  {
    tmp += i;
    outputScalar[i] = float(tmp)/float(i);
  }
  scalarTime.Stop();
  cout << "scalarTime (ms): " << scalarTime.RealTime()*1000  << endl; 
  


  // WORSE OMP SOLUTION: IMPOSE ORDERED FOR LOOP
  tmp = 0;
  TStopwatch orderedTime;
  #pragma omp parallel num_threads(NThreads)
  {
    #pragma omp for ordered schedule(static,1)
    for(int i=1; i<N; i++){
      # pragma omp ordered
      {
        tmp += i;
        outputParallel[i] = float(tmp)/float(i);
      }
    }
  }
  orderedTime.Stop();
  cout << "orderedTime (ms): " << orderedTime.RealTime()*1000  << endl; 


  // BETTER OMP SOLUTION: WRITE TMP AS FUNCTION OF I
  TStopwatch functionalTime;
  tmp = 0;
  #pragma omp parallel num_threads(NThreads)
  {
    #pragma omp for 
    for(int i=1; i<N; i++)
    {
      float tmp = (1+i)*i/2;
      outputParallel[i] = float(tmp)/float(i);
    }
  }
  functionalTime.Stop();
  cout << "functionalTime (ms): " << functionalTime.RealTime()*1000  << endl; 







  

  // check the output array
  bool ok = 1;
  for(int i=1; i<N; i++)
    if(fabs(outputScalar[i] - outputParallel[i]) > 1.e-8) ok = 0;
  
  if(ok)
    std::cout << "Arrays are the same." << std::endl;
  else
    std::cout << "ERROR!!! The output array is not correct!" << std::endl;
  
  return 0;
}
