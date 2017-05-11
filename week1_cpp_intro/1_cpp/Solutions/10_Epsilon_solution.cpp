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

template <class T>
T Epsilon() {
//  cout << " (Type size: " << sizeof(T) << ") ";
	
  const T one = 1;
  T e = one;
  
  for( T oneP = one + e/2; abs(oneP - one) > 0; oneP = one + e/2 ) {
    e = e/2;
  }
  return e;
}

int main () {
  cout << "Machine epsilon for float =       ";
  cout << Epsilon<float>() << endl;
  cout << "1 + e - 1 = " << 1 + Epsilon<float>() - 1 << endl;

  cout << "Machine epsilon for double =      ";
  cout << Epsilon<double>() << endl;
  cout << "1 + e - 1 = " << 1 + Epsilon<double>() - 1 << endl;

  cout << "Machine epsilon for long double = ";
  cout << Epsilon<long double>() << endl;
  cout << "1 + e - 1 = " << 1 + Epsilon<long double>() - 1 << endl;

  cout << "Machine epsilon for int =         ";
  cout << Epsilon<int>() << endl;
  cout << "1 + e - 1 = " << 1 + Epsilon<int>() - 1 << endl;
	
  return 0;
}


