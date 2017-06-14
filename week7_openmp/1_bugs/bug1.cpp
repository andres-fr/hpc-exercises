/* ==================================================
   Exersice. Hello World. OpenMP
   Authors: I.Kulakov; M.Zyzak
   ================================================== */

// to compile and run the program type: g++ bug1.cpp -O3 -fopenmp -o bug1.out; ./bug1.out


// g++ -O3 -fopenmp -std=c++14 -Wall -Wextra /home/afr/SS17/hpc-praktikum/week7_openmp/1_bugs/bug1.cpp -o bug1 && ./bug1


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
  // PRIVATE creates a new copy of N for each thread, value is uninitialized
  //  shared means that the variable N is shared among threads, can't be applied to "omp for" loop
  // firstprivate is like private, but initialized with the value of the master thread. WE WANT THIS: change private for firstprivate
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
