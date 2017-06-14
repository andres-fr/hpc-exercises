/* ==================================================
   Exersice. Hello World. OpenMP
   Authors: I.Kulakov; M.Zyzak
   ================================================== */

// to compile and run the program type: g++ bug4.cpp -O3 -fopenmp -o bug4.out; ./bug4.out

// The program calculates a sum of N elements and calculates the fraction of each element in this sum.
// Find a bug and correct it

#include "omp.h"
#include <stdlib.h> // rand
#include <cmath>

#include <iostream>
using namespace std;

static const int NThreads = 10;

void CalcuateSum(const float* input, int iStart, int iEnd, float& sum)
{
  for(int i = iStart; i<iEnd; i++)
    sum += input[i];
}

int main() {

  int N = 10000;

  float *input = 0, *outputScalar=0, *outputParallel = 0;

  input = new float[N];
  outputScalar = new float[N];
  outputParallel = new float[N];

  // fill an input matrix with by random numbers
  for( int i = 0; i < N; i++ ) {
    input[i] = float(rand())/float(RAND_MAX); // put a random value, from 0 to 1
  } 

  int tmp = 0;
  for(int i=1; i<N; i++)
  {
    tmp += i;
    outputScalar[i] = float(tmp)/float(i);
  }

  float sum = 0;
  {
    CalcuateSum(input,0, N, sum);
    
    #pragma omp for
    for(int i=0; i<N; i++)
      outputScalar[i] = input[i]/sum;
  }
  
  sum = 0;
  #pragma omp parallel num_threads(NThreads)
  {
    int id, i, Nthrds, istart, iend;
    id = omp_get_thread_num();
    Nthrds = omp_get_num_threads();
    istart= id * N / Nthrds;
    iend= (id+1) * N / Nthrds;
    if (id == Nthrds-1)
      iend= N;
    float sumLocal = 0;
    
    CalcuateSum(input,istart, iend, sumLocal);

    #pragma omp atomic
    sum += sumLocal;

    #pragma omp barrier
    
    #pragma omp for
    for(int i=0; i<N; i++)
      outputParallel[i] = input[i]/sum;
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
