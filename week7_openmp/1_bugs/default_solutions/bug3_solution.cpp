/* ==================================================
   Exersice. Hello World. OpenMP
   Authors: I.Kulakov; M.Zyzak
   ================================================== */

// to compile and run the program type: g++ bug3.cpp -O3 -fopenmp -o bug3.out; ./bug3.out

// The program calculates a sum of N elements and calculates the fraction of each element in this sum.
// Find a bug and correct it

#include "omp.h"
#include <stdlib.h> // rand
#include <cmath>

#include <iostream>
using namespace std;

static const int NThreads = 100;

int main() {

  int N = 1000;
  
  float *input=0, *outputScalar=0, *outputParallel=0;
  
  input = new float[N];
  outputScalar = new float[N];
  outputParallel = new float[N];  
  
    // fill an input matrix with by random numbers
  for( int i = 0; i < N; i++ ) {
    input[i] = float(rand())/float(RAND_MAX); // put a random value, from 0 to 1
  } 

  float sum = 0;

  //scalar code
  {
    for(int i=0; i<N; i++)
      sum += input[i];
    
    for(int i=0; i<N; i++)
      outputScalar[i] = input[i]/sum;
  }
  
  sum = 0;
  {

    for(int i=0; i<N; i++)
    {
      sum += input[i];
    }
    
    #pragma omp parallel firstprivate(N) num_threads(NThreads)
    #pragma omp for 
    for(int i=0; i<N; i++)
      outputParallel[i] = input[i]/sum;
  }

  // check the output array
  bool ok = 1;
  for(int i=0; i<N; i++)
    if(fabs(outputScalar[i] - outputParallel[i]) > 1.e-8) { ok = 0; }
  
  if(ok)
    std::cout << "Arrays are the same." << std::endl;
  else
    std::cout << "ERROR!!! The output array is not correct!" << std::endl;
  
  return 0;
}
