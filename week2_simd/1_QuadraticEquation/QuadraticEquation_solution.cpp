/// The maximum root of a qudratic equation a*x^2+b*x+c = 0
///
/// @author I.Kulakov, M.Zyzak
/// @e-mail I.Kulakov@gsi.de, M.Zyzak@gsi.de
/// 
/// use "g++ QuadraticEquation_solution.cpp -O3 -fno-tree-vectorize -g -msse; ./a.out" to run

/// Task:
/// 1. Write the SIMD code for the root calculation using SIMD intrinsics and copying the data to SIMD vectors.
/// 2. Write the SIMD code using SIMD intrinsics and casting the data from the scalar arrays to SIMD vectors (use reinterpret_cast for this). Compare the time with a previous task.
/// 3. Write the SIMD code using header files and copying the data to SIMD vectors. Compare the time with previous tasks.
/// 4. Write the SIMD code using header files and casting the data from the scalar arrays to SIMD vectors (use reinterpret_cast for this). Compare the time with previous tasks.
/// 5. Put NVectors = 10000000; and NIterOut=10. Compare times and speed up factors with the previous results.

/// Note:
/// __m128 - SIMD vector
/// SIMD intrinsics:
/// _mm_set_ps(f3,f2,f1,f0) - write 4 floats into the SIMD vector
/// note, that the order of entries is inversed
/// _mm_set_ps1(a) - write float "a" into the SIMD vector
/// _mm_add_ps(a,b) - a+b
/// _mm_sub_ps(a,b) - a-b
/// _mm_mul_ps(a,b) - a*b
/// _mm_div_ps(a,b) - a/b
/// _mm_sqrt_ps(a) - sqrt(a)
/// type2* pointer2 = reinterpret_cast<type2*>( pointer1 ) - change pointer type

#include "xmmintrin.h"
#include "../../downloads/vectors/P4_F32vec4.h" // simulation of the SSE instruction
//#include "../../../vectors/PSEUDO_F32vec4.h" // simulation of the SSE instruction with vector width = 4
//#include "../../../vectors/PSEUDO_F32vec1.h" // simulation of the SSE instruction with vector width = 1

#include <cmath>
#include <iostream>
using namespace std;

#include <stdlib.h> // rand
#include "../../downloads/TStopwatch.h"

static const int NVectors = 10000;
static const int N = NVectors*fvecLen;

static const int NIterOut=1000;


void CheckResults(const float* yScalar, const float* ySIMD, const int NSIMD)
{
  bool ok = 1;
  for(int i=0; i<N; i++)
    if( fabs(yScalar[i] - ySIMD[i]) > yScalar[i]*0.01 )
    {
      ok = 0; 
      std::cout << i<<" " << yScalar[i] << " " << ySIMD[i] << " " << fabs(yScalar[i] - ySIMD[i])<<std::endl;
    }
  if(!ok)
    std::cout << "ERROR! SIMD" << NSIMD << " and scalar results are not the same." << std::endl;
  else
    std::cout << "SIMD" << NSIMD << " and scalar results are the same." << std::endl;
}

int main() {

  //input data
  float* a = (float*) _mm_malloc(sizeof(float)*N,16);// __attribute__ ((aligned(16)));
  float* b = (float*) _mm_malloc(sizeof(float)*N,16);// __attribute__ ((aligned(16)));
  float* c = (float*) _mm_malloc(sizeof(float)*N,16);// __attribute__ ((aligned(16)));
  //output data
  float* x = (float*) _mm_malloc(sizeof(float)*N,16);// __attribute__ ((aligned(16)));
  float* x_simd1 = (float*) _mm_malloc(sizeof(float)*N,16);// __attribute__ ((aligned(16)));
  float* x_simd2 = (float*) _mm_malloc(sizeof(float)*N,16);// __attribute__ ((aligned(16)));
  float* x_simd3 = (float*) _mm_malloc(sizeof(float)*N,16);// __attribute__ ((aligned(16)));
  float* x_simd4 = (float*) _mm_malloc(sizeof(float)*N,16);// __attribute__ ((aligned(16)));

  // fill parameters by random numbers
  for( int i = 0; i < N; i++ ) {
    a[i] = 0.01 + float(rand())/float(RAND_MAX); // put a random value, from 0.01 to 1.01 (a has not to be equal 0)
    b[i] = float(rand())/float(RAND_MAX);
    c[i] = -float(rand())/float(RAND_MAX);
  }

    /// -- CALCULATE --

  // scalar calculations
  TStopwatch timerScalar;
  for(int io=0; io<NIterOut; io++)
    for(int i=0; i<N; i++)
    {
      float det = b[i]*b[i] - 4*a[i]*c[i];
      x[i] = (-b[i]+sqrt(det))/(2*a[i]);
    }
  timerScalar.Stop();

  // SIMD clculations with SIMD intrinsics and data copy
  TStopwatch timerSIMD;
  for(int io=0; io<NIterOut; io++)
    for(int i=0; i<NVectors; i++)
    {
      __m128 aV = _mm_set_ps(a[i*fvecLen+3],a[i*fvecLen+2],a[i*fvecLen+1],a[i*fvecLen]);
      __m128 bV = _mm_set_ps(b[i*fvecLen+3],b[i*fvecLen+2],b[i*fvecLen+1],b[i*fvecLen]);
      __m128 cV = _mm_set_ps(c[i*fvecLen+3],c[i*fvecLen+2],c[i*fvecLen+1],c[i*fvecLen]);
      
      const __m128 det = _mm_sub_ps(_mm_mul_ps(bV,bV) , _mm_mul_ps(_mm_set_ps1(4),_mm_mul_ps(aV,cV) ) );
      __m128 xV = _mm_div_ps(_mm_sub_ps(_mm_sqrt_ps(det),bV),_mm_mul_ps(_mm_set_ps1(2),aV));

      // copy output data      
      for(int iE=0; iE<fvecLen; iE++)
        x_simd1[i*fvecLen+iE] = (reinterpret_cast<float*>(&xV))[iE];
    }
  timerSIMD.Stop();

  // SIMD clculations with SIMD intrinsics and reinterpret_cast  
  TStopwatch timerSIMD2;
  for(int io=0; io<NIterOut; io++)
    for(int i=0; i<N; i+=fvecLen)
    {
      __m128& aV = (reinterpret_cast<__m128&>(a[i]));
      __m128& bV = (reinterpret_cast<__m128&>(b[i]));
      __m128& cV = (reinterpret_cast<__m128&>(c[i]));

      __m128& xV = (reinterpret_cast<__m128&>(x_simd2[i]));

      const __m128 det = _mm_sub_ps(_mm_mul_ps(bV,bV) , _mm_mul_ps(_mm_set_ps1(4),_mm_mul_ps(aV,cV) ) );
      xV = _mm_div_ps(_mm_sub_ps(_mm_sqrt_ps(det),bV),_mm_mul_ps(_mm_set_ps1(2),aV));
    }
  timerSIMD2.Stop();

  // SIMD clculations with headers and data copy 
  TStopwatch timerSIMD3;
  for(int io=0; io<NIterOut; io++)
  {
    for(int i=0; i<NVectors; i++)
    {
      // copy input data
      fvec aV = fvec(a[i*fvecLen],a[i*fvecLen+1],a[i*fvecLen+2],a[i*fvecLen+3]);
      fvec bV = fvec(b[i*fvecLen],b[i*fvecLen+1],b[i*fvecLen+2],b[i*fvecLen+3]);
      fvec cV = fvec(c[i*fvecLen],c[i*fvecLen+1],c[i*fvecLen+2],c[i*fvecLen+3]);
      
      const fvec det = bV*bV - 4*aV*cV;
      fvec xV = (-bV+sqrt(det))/(2*aV);

      // copy output data      
      for(int iE=0; iE<fvecLen; iE++)
        x_simd3[i*fvecLen+iE] = xV[iE];      
    }
  }
  timerSIMD3.Stop();

  // SIMD clculations with headers and reinterpret_cast  
  TStopwatch timerSIMD4;
  for(int io=0; io<NIterOut; io++)
    for(int i=0; i<N; i+=fvecLen)
    {
      fvec& aV = (reinterpret_cast<fvec&>(a[i]));
      fvec& bV = (reinterpret_cast<fvec&>(b[i]));
      fvec& cV = (reinterpret_cast<fvec&>(c[i]));
      fvec& xV = (reinterpret_cast<fvec&>(x_simd4[i]));

      const fvec det = bV*bV - 4*aV*cV;
      xV = (-bV+sqrt(det))/(2*aV);
    }
  timerSIMD4.Stop();

  double tScal = timerScalar.RealTime()*1000;
  double tSIMD1 = timerSIMD.RealTime()*1000;
  double tSIMD2 = timerSIMD2.RealTime()*1000;
  double tSIMD3 = timerSIMD3.RealTime()*1000;
  double tSIMD4 = timerSIMD4.RealTime()*1000;
  cout << "Time scalar: " << tScal << " ms " << endl;
  cout << "Time SIMD1:   " << tSIMD1 << " ms, speed up " << tScal/tSIMD1 << endl;
  cout << "Time SIMD2:   " << tSIMD2 << " ms, speed up " << tScal/tSIMD2 << endl;
  cout << "Time SIMD3:   " << tSIMD3 << " ms, speed up " << tScal/tSIMD3 << endl;
  cout << "Time SIMD4:   " << tSIMD4 << " ms, speed up " << tScal/tSIMD4 << endl;

  //compare SIMD and scalar results
  CheckResults(x,x_simd1,1);
  CheckResults(x,x_simd2,2);
  CheckResults(x,x_simd3,3);
  CheckResults(x,x_simd4,4);

  return 1;
}
