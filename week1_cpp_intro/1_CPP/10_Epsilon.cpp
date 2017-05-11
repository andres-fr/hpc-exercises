/* ==================================================
   Exersice. Templates.
   Authors: I.Kulakov; M.Zyzak
   ================================================== */

// The machine epsilon can be found as the smallest value of E, such that (1 + E) is not equal to 1.
// see http://en.wikipedia.org/wiki/Machine_epsilon for more information

// Estimate (with precision < 100%) the machine epsilon for float, double, long double types. Use templates.

#include <iostream>
#include <iomanip>
#include <cmath>
using namespace std;


// TODO template function
template <typename T> // no <class T> needed
T Epsilon() {
  const T one = 1;
  T e = one;
  // bitshift e to the right until becomes 0
  for (T oneP = one + e/2; abs(oneP-one)>0; oneP=one+e/2){
    e = e/2;
  }
  return e;
}

int main () {
  cout << "epsilon for float: " << Epsilon<float>() << endl;
  cout << "epsilon for double: " << Epsilon<double>() << endl;
  cout << "epsilon for long double: " << Epsilon<long double>() << endl;

  return 0;
}


