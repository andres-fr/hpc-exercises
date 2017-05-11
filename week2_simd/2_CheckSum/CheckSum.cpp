  /// Check sum calculation
  ///
  /// @authors: I.Kulakov; M.Zyzak
  /// @e-mail I.Kulakov@gsi.de; M.Zyzak@gsi.de
  /// 
  /// use "g++ CheckSum.cpp -O3 -o a.out -fno-tree-vectorize; ./a.out" to run

 // make calculation parallel: a) using SIMD instructions, b) using usual instructions!

#include "../../libs/vectors/P4_F32vec4.h" // simulation of the SSE instruction
#include "../../libs/TStopwatch.h"

#include <iostream>
using namespace std;

#include <stdlib.h> // rand

const int NIter = 100;

const int N = 4000000; // matrix size. Has to be dividable by 4.
unsigned char str[N] __attribute__ ((aligned(16))); //*****************

template< typename T >
T Sum(const T* data, const int N)
{
    T sum = 0;

    for ( int i = 0; i < N; ++i )
      sum = sum ^ data[i];
    return sum;
}

int main() {

    // fill string by random values
  for( int i = 0; i < N; i++ ) {
    str[i] = 256 * ( double(rand()) / RAND_MAX ); // put a random value, from 0 to 255
  } 
  
    /// -- CALCULATE --
  
    /// SCALAR
  
  unsigned char sumS = 0;
  TStopwatch timerScalar;
  for( int ii = 0; ii < NIter; ii++ )
    sumS = Sum<unsigned char>( str, N );
  timerScalar.Stop();
  
    /// SIMD

  unsigned char sumV = 0;
  unsigned char* sumIter;
  TStopwatch timerSIMD;

  //*************************************
  
  for( int ii = 0; ii < NIter; ii++ ){
    __m128 resVec = _mm_set_ps1(0);
    for(int i=0; i<N; i+=16){
      __m128& x = (reinterpret_cast<__m128&>(str[i]));
      resVec = _mm_xor_ps(resVec,x);
    }
    sumIter = reinterpret_cast<unsigned char*>(&resVec);
    sumV = sumIter[0];
    for (int i=1; i<16; ++i){
      sumV ^= sumIter[i];
    }
  }
  //****************************************

  timerSIMD.Stop();

  // SCALAR INTEGER
  unsigned char sumI = 0;
  unsigned char* temp;
  TStopwatch timerINT;
  for( int i = 0; i < NIter; ++i) {
    int x = Sum<int>(reinterpret_cast<int*>(str), N/4);
    temp = reinterpret_cast<unsigned char*>(&x);
    sumI = temp[0];
    for ( int j = 1; j < 4; ++j){
      sumI ^= temp[j];
    }
  }
  timerINT.Stop();


    /// -- OUTPUT --
  
  double tScal = timerScalar.RealTime()*1000;
  double tINT = timerINT.RealTime()*1000;
  double tSIMD = timerSIMD.RealTime()*1000;


  
  cout << "SumS>>>>>>>" << sumS << endl;
  cout << "SumI>>>>>>" << sumI << endl;
  
  cout << "Time scalar: " << tScal << " ms " << endl;
  cout << "Time INT:   " << tINT << " ms, speed up " << tScal/tINT << endl;
  cout << "Time SIMD:   " << tSIMD << " ms, speed up " << tScal/tSIMD << endl;

  // cout << static_cast<int>(sumS) << " " << static_cast<int>(sumV) << endl;
  if(sumV == sumS && sumI == sumS )
    std::cout << "Results are the same." << std::endl;
  else
    std::cout << "ERROR! Results are not the same." << std::endl;
  
  return 1;
}
