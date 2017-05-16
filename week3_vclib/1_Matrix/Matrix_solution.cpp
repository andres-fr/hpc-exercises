  /// Element-wise square root of matrix
  ///
  /// @authors: I.Kulakov; M.Zyzak
  /// @e-mail I.Kulakov@gsi.de; M.Zyzak@gsi.de
  /// 
  /// use "g++ Matrix_solution.cpp -O3 -lVc; ./a.out" to run

// Finish SIMDized version. Compare results and time.

#include "../../../vectors/P4_F32vec4.h" // overloading of the SSE instruction
#include "../../../TStopwatch.h"

#include <Vc/Vc>
using namespace Vc;

#include <iostream>
using namespace std;

#include <stdlib.h> // rand


const int N = 1000; // matrix size. Has to be dividable by 4.

const int NIter = 100; // repeat calculations many times in order to neglect memory read time

float a[N][N] __attribute__ ((aligned(16)));
float c[N][N] __attribute__ ((aligned(16)));
float c_simd[N][N] __attribute__ ((aligned(16)));
float c_simdVc[N][N] __attribute__ ((aligned(16)));

template<typename T> // required calculations
T f(T x) {
  return sqrt(x);
}

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
        c[i][j] = f(a[i][j]);
      }
    }
  timerScalar.Stop();
  
    /// SIMD VECTORS
  TStopwatch timerSIMD;
  for( int ii = 0; ii < NIter; ii++ )
    for( int i = 0; i < N; i++ ) {
      for( int j = 0; j < N; j+=fvecLen ) {
          fvec &aVec = (reinterpret_cast<fvec&>(a[i][j]));
          fvec &cVec = (reinterpret_cast<fvec&>(c_simd[i][j]));
          cVec = f(aVec);
      }
    }
  timerSIMD.Stop();

    /// Vc
  TStopwatch timerVc;
  for( int ii = 0; ii < NIter; ii++ )
    for( int i = 0; i < N; i++ ) {
      for( int j = 0; j < N; j+=float_v::Size ) {
          float_v &aVec = (reinterpret_cast<float_v&>(a[i][j]));
          float_v &cVec = (reinterpret_cast<float_v&>(c_simdVc[i][j]));
          cVec = f(aVec);
      }
    }
  timerVc.Stop();
  
  double tScal  = timerScalar.RealTime()*1000;
  double tSIMD1 = timerSIMD.RealTime()*1000;
  double tVc    = timerVc.RealTime()*1000;
  
  cout << "Time scalar:  " << tScal << " ms " << endl;
  cout << "Time headers: " << tSIMD1 << " ms, speed up " << tScal/tSIMD1 << endl;
  cout << "Time Vc:      " << tVc << " ms, speed up " << tScal/tVc << endl;

  CheckResults(c,c_simd);
  CheckResults(c,c_simdVc);
    
  return 1;
}
