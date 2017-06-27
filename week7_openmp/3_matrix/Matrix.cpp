  /// Element-wise square root of matrix
  ///
  /// @authors: I.Kulakov; M.Zyzak
  /// @e-mail I.Kulakov@gsi.de; M.Zyzak@gsi.de
  /// 
  /// use "g++ Matrix.cpp -O3 -fopenmp -o mat -lVc; ./mat" to run

// g++ -O3 -fopenmp -std=c++14 -Wall -Wextra -I/opt/Vc/include -L/opt/Vc/lib Matrix.cpp -o Matrix -lVc && ./Matrix


// Parallelize the SIMDized version between cores isng OpenMP. Compare the results and time.

#include "omp.h"

#include "../../libs/TStopwatch.h"

// #include "../../libs/vectors/P4_F32vec4.h" // overloading of the SSE instruction
#include <Vc/Vc>
using namespace Vc;

#include <iostream>
using namespace std;

#include <stdlib.h> // rand


const int N = 1000; // matrix size. Has to be dividable by 4.

const int NIter = 4000; // repeat calculations many times in order to neglect memory read time

float a[N][N] __attribute__ ((aligned(16)));
float c[N][N] __attribute__ ((aligned(16)));
float c_simd[N][N] __attribute__ ((aligned(16)));
float c_omp[N][N] __attribute__ ((aligned(16)));

void CheckResults(const float a1[N][N], const float a2[N][N])
{
  bool ok = 1;
  for(int i=0; i<N; i++)
    for(int j=0; j<N; j++)
      if( fabs(a1[i][j] - a2[i][j]) > 1.e-8 ) ok = 0;

  if(ok)
    std::cout << "SIMD and scalar results are the same." << std::endl;
  else
    std::cout << "ERROR! SIMD and scalar results are not the same." << std::endl;
}

int main() {

    // fill classes by random numbers
  for( int i = 0; i < N; i++ ) {
    for( int j = 0; j < N; j++ ) {
      a[i][j] = float(rand())/float(RAND_MAX); // put a random value, from 0 to 1
    }
  } 
  
    /// -- CALCULATE --
    /// SCALAR
  TStopwatch timerScalar;
  for( int ii = 0; ii < NIter; ii++ )
    for( int i = 0; i < N; i++ ) {
      for( int j = 0; j < N; j++ ) {
        c[i][j] = sqrt(a[i][j]);
      }
    }
  timerScalar.Stop();
  
   /// Vc
  TStopwatch timerVc;
  for( int ii = 0; ii < NIter; ii++ )
    for( int i = 0; i < N; i++ ) {
      for( int j = 0; j < N; j+=float_v::Size ) {
          float_v &aVec = (reinterpret_cast<float_v&>(a[i][j]));
          float_v &cVec = (reinterpret_cast<float_v&>(c_simd[i][j]));
          cVec = sqrt(aVec);
      }
    }
  timerVc.Stop();
  
   ///OpenMP
  // TStopwatch timerOMP;
  int NThreads = 1; // best is 1...
  double start_time = omp_get_wtime();
  for( int ii = 0; ii < NIter; ii++ ){ // repeat several times to improve time measurement precision
    #pragma omp parallel for num_threads(NThreads) // ************** omp_get_num_procs()?? the more threads, slower it gets... why?
    for( int i = 0; i < N; i++ ) {
      for( int j = 0; j < N; j+=float_v::Size ) {
        float_v &aVec = (reinterpret_cast<float_v&>(a[i][j]));
          float_v &cVec = (reinterpret_cast<float_v&>(c_omp[i][j]));
          cVec = sqrt(aVec);
      }
    }
  }
  double time_mp = (omp_get_wtime() - start_time) * 1000; // miliseconds
// timerOMP.Stop();

double tScal  = timerScalar.RealTime()*1000;
double tVc    = timerVc.RealTime()*1000;
double tOMP   = time_mp;//timerOMP.RealTime()*1000;
  
cout << "Time scalar:  " << tScal << " ms " << endl;
cout << "Time Vc:      " << tVc << " ms, speed up " << tScal/tVc << endl;
cout << "Time OpenMP with "<< NThreads <<" threads: " << tOMP << " ms, speed up " << tScal/tOMP << endl;

CheckResults(c,c_simd);
CheckResults(c,c_omp);

return 1;

}
