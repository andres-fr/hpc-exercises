/* ==================================================
   Exersice. Hello World. OpenMP
   Authors: I.Kulakov; M.Zyzak
   ================================================== */

// to compile and run the program type: g++ bug2.cpp -O3 -fopenmp -o bug2.out; ./bug2.out

// Find a bug and correct it

#include "omp.h"
#include <stdlib.h> // rand
#include <cmath>

#include <iostream>
using namespace std;

static const int NThreads = 10;

int main() {

  int N = 10000;
  
  float *outputScalar=0, *outputParallel = 0;
  
  outputScalar = new float[N];
  outputParallel = new float[N];

  
  int tmp = 0;
  for(int i=1; i<N; i++)
  {
    tmp += i;
    outputScalar[i] = float(tmp)/float(i);
  }

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
