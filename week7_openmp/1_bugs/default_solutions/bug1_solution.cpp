/* ==================================================
   Exersice. Hello World. OpenMP
   Authors: I.Kulakov; M.Zyzak
   ================================================== */

// to compile and run the program type: g++ bug1.cpp -O3 -fopenmp -o bug1.out; ./bug1.out

// The program should copy one array to another in parallel
// Find a bug and correct it

#include "omp.h"
#include <stdlib.h> // rand

#include <iostream>
using namespace std;

static const int NThreads = 10;

int main() {

  int N = 1000;
  
  float *input=0, *output=0;
  
  input = new float[N];
  output = new float[N];
  
    // fill an input matrix with by random numbers
  for( int i = 0; i < N; i++ ) {
    input[i] = float(rand())/float(RAND_MAX); // put a random value, from 0 to 1
  } 

  #pragma omp parallel firstprivate(N) num_threads(NThreads)
  {
    #pragma omp for 
    for(int i=0; i<N; i++)
      output[i] = input[i];
  }

  // check the output array
  bool ok = 1;
  for(int i=0; i<N; i++)
    if(output[i] != input[i]) ok = 0;
  
  if(ok)
    std::cout << "Arrays are the same." << std::endl;
  else
    std::cout << "ERROR!!! The output array is not correct!" << std::endl;
  
  return 0;
}
