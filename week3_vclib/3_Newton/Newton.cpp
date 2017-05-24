  /// Iterative method for finding approximations to the roots (or zeroes) of a function.
  /// http://en.wikipedia.org/wiki/Newton's_method
  ///
  /// @authors: I.Kulakov; M.Zyzak
  /// @e-mail I.Kulakov@gsi.de
  /// 
  /// use "g++ Newton.cpp -O3 -lVc; ./a.out" to run
 
  /// TODO: SIMDIZE using Vector classes. Compare results and time

  // Hints: use float_v, float_m, float_m::isEmpty(), mask operators


// andres local run version:
// g++ -O3 -fno-tree-vectorize -g -msse -std=c++11 -I/opt/Vc/include -L/opt/Vc/lib  Newton.cpp -o test -lVc && ./test


#include <cmath>
#include <stdlib.h> // rand

#include <iostream> // cout
using namespace std;

#include "../../libs/TStopwatch.h"

#include <Vc/Vc> // Vector classes 
using namespace Vc;

const int N = 100000;
float par1[N], par2[N] __attribute__ ((aligned(16))); // parameters arrays
float root[N] __attribute__ ((aligned(16))); // output array of the root of the function
const float P = 1e-5; // precision of root computation

  // the two-parameters function to find root (polynomial)
template<class T>
T F(const T& x, const T& p1, const T& p2)
{
  const T x2 = x * x;
  return p1 * x2*x + p2 * x2 + 6 * x + 80;
  
  return  80 + x * (6 + x * (p2 + x*p1));    // p1*x^3 + p2*x^2 + 6x + 80
}

  // the derivative of the function
template<class T>
T Fd(const T& x, const T& p1, const T& p2)
{
  //return 3*p1 * x*x + 2*p2 * x + 6;
  return 6 + x*(2*p2 + x * (3*p1)); // 3p1*x^2 + 2p2*x + 6
}

float FindRootScalar(const float& p1, const float& p2)
{
  float x = 1, x_new = 0;
  for( ; abs((x_new - x)/x_new) > P; ) {
    x = x_new;
    x_new = x - F(x,p1,p2) / Fd(x,p1,p2);
  }
  return x_new;
}


float_v FindRootVector(const float_v& p1, const float_v& p2)
{
  //TODO write a vectorized code using Vc
  Vc::float_v vx = 1.f, vx_new = 0.f;
  float_m mask(true);
  for( ; !mask.isEmpty(); ) {
    vx = vx_new;
    vx_new(mask) = vx - F(vx,p1,p2) / Fd(vx,p1,p2);
    mask = abs((vx_new - vx)/vx_new) > P;
  }
  return vx_new;
}


bool CheckResults(float* r)
{
  bool ok = true;
  for( int i = 0; i < N; ++i ) {
    ok &= fabs(F(root[i], par1[i], par2[i])/Fd(root[i], par1[i], par2[i])) < P;
  }
  return ok;
};

bool CompareResults(float* r1, float* r2)
{
  bool ok = true;
  for( int i = 0; i < N; ++i ) {
    ok &= fabs(r1[i] - r2[i]) < 1e-7;
  }
  return ok;
};

int main()
{
    // set random values for parameters
  for( int i = 0; i < N; ++i ) {
    par1[i] = 1 + double(rand())/double(RAND_MAX); // from 1 to 2
    par2[i] = double(rand())/double(RAND_MAX);     // from 0 to 1
  }

    /// -----------  Scalar part 
  
  TStopwatch timer;
  for( int i = 0; i < N; ++i ) {
    root[i] = FindRootScalar( par1[i], par2[i] );
  }
  timer.Stop();

  cout << "Scalar part:" << endl;
  if ( CheckResults(root) )
    cout << "Results are correct!" << endl;
  else
    cout << "Results are INCORRECT!" << endl;
  cout << "Time: " << timer.RealTime()*1000 << " ms " << endl;
  float timeS = timer.RealTime();
  
    /// -----------  SIMD part 
  
  const int Nv = N / float_v::Size;
  float_v par1_v[Nv], par2_v[Nv];
  float_v root_v[Nv];
  float   root2[N];

  
    // copy input
  for( int i = 0; i < Nv; ++i ) {
    par1_v[i].load( par1 + i * float_v::Size);
    par2_v[i].load( par2 + i * float_v::Size);
  }


  // compute
  timer.Start();
  for( int i = 0; i < Nv; ++i ) {
    root_v[i] = FindRootVector(par1_v[i], par2_v[i] );
  }
  timer.Stop();
  
    // copy output
  // TODO copy the data back to root_v (you can use store() vunction of Vc)
  for( int i = 0; i < Nv; ++i ) {
    root_v[i].store( root2 + i * float_v::Size );
  }

  cout << "SIMD part:" << endl;
  if ( CompareResults(root, root2) )
  //if ( CheckResults(root2) )
    cout << "Results are the same!" << endl;
  else
    cout << "Results are NOT the same!" << endl;
  cout << "Time: " << timer.RealTime()*1000 << " ms " << endl;
  float timeV = timer.RealTime();
  
  cout << "Speed up: " << timeS/timeV << endl;
  
  return 1;
}
