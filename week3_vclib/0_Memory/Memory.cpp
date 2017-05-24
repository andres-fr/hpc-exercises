  /// Using Memory
  ///
  /// @authors: I.Kulakov; M.Zyzak
  /// @e-mail I.Kulakov@gsi.de; M.Zyzak@gsi.de
  /// based on Vc Manual: http://code.compeng.uni-frankfurt.de/docs/Vc-0.4.0/
  /// 
  /// use "g++ Memory.cpp -O3 -lVc; ./a.out" to run


// andres local run version:
// g++ -O3 -std=c++11 -I/opt/Vc/include -L/opt/Vc/lib  Memory.cpp -o test -lVc && ./test

// Finish SIMDized version. Compare results and time.

#include "../../libs/TStopwatch.h" // added to benchmark

#include <iostream>
using namespace std;

#include <Vc/Vc>
// using namespace Vc;

int main() {

  const int NV = 300;
  const int N = 1100; // N is not allowed to be bigger than NV*4!!
  
  int in[N]; // copy from
  Vc::int_v x[NV]; // int_v has 4 ints for SIMD=128b register, x has 12 ints
  int out[N]; // copy in

  for( int i = 0; i < N; ++i)
    in[i] = i;
  
    /// -- Memory --
    
  {
    Vc::Memory<Vc::int_v, N> array; // aligned memory


      // scalar access:
    for (int i = 0; i < array.entriesCount(); ++i) {
      array[i] = in[i];     // write
      //cout << ">>" << array[i] << endl;
    }
    TStopwatch timerMemory;
      // vector access:
    for (int i = 0; i < array.vectorsCount(); ++i) {
      x[i] = array.vector(i); // read
      //cout << i << ">>" << x[i] << endl;
    }

      // do something
    for (int i = 0; i < array.vectorsCount(); ++i) {
      x[i] *= 2;
    }
  
    for (int i = 0; i < array.vectorsCount(); ++i) {
      array.vector(i) = x[i];       // write back
    }
    timerMemory.Stop();
      // scalar access:
    for (int i = 0; i < array.entriesCount(); ++i) {
      out[i] = array[i]; // read
    }

    
  double tMemory = timerMemory.RealTime()*1000;
  cout << "Elapsed time Memory: " << tMemory << " ms " << endl;
  }

  
  // cout << "Memory" << endl;
  // for( int i = 0; i < N; ++i)
  //   cout << out[i] << " ";
  // cout << endl;
  

  
    /// -- Load & Store s--
  TStopwatch timerLoadStore;

  {
    //int array[12] __attribute__ ((aligned(16)));

    for (int i = 0; i < NV; ++i) {
      x[i].load( in + i * Vc::int_v::Size, Vc::Aligned ); // copy the contents of "in" into x, parallelized
    }

      // do something
    for (int i = 0; i < NV; ++i) {
      x[i] *= 2;
    }
    
    for (int i = 0; i < NV; ++i) {
      x[i].store( out + i * Vc::int_v::Size, Vc::Aligned );  // copy the contents of x into "out", parallelized
    }
  }

  timerLoadStore.Stop();
  double tLoadStore = timerLoadStore.RealTime()*1000;
  cout << "Load & Store" << endl;
  // for( int i = 0; i < N; ++i)
  //   cout << out[i] << " ";
  // cout << endl;
  cout << "Elapsed time LoadStore: " << tLoadStore << " ms " << endl;

  return 1;
}
