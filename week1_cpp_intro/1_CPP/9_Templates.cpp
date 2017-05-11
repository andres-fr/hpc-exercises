/* ==================================================
   Exersice. Templates.
   Authors: I.Kulakov; M.Zyzak
   ================================================== */

// Run. Explain the output.

#include <iostream>
using namespace std;

template <typename T>
T GetMax (T a, T b) {
  T result;
  if (a > b)   result = a;
  else         result = b;
  return result;
}

int main () {
  int i=5, j=6;
  double l=9.2, m=2e9;
  cout << GetMax<int>(i,j) << endl;
  cout << GetMax<double>(l,m) << endl;
  cout << "float: " <<  GetMax<float>(l,m) << endl;
  cout << "int: "   <<  GetMax<int>(l,m) << endl;
  cout << "short: " <<  GetMax<short>(l,m) << endl;
  cout << "char: "  <<  GetMax<char>(l,m) << endl;
  return 0;
}

