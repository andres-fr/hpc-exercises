/* ==================================================
   Exersice. Pointers.
   Authors: I.Kulakov; M.Zyzak
   ================================================== */

// Will this program work? Find and fix 2 bugs.

#include<iostream>
using namespace std;

void piPointer1(float* pi) {
  *pi = 3.14;
}

float* piPointer2() {
  float* pi = new float;
  *pi = 3.1415;
  return pi;
}

int main() {
  float* pi1 = new float;
  piPointer1(pi1);
  cout << *pi1 << endl;
  delete pi1;
    
  float* pi2 = piPointer2();
  cout << *pi2 << endl;
  delete pi2;
    
  return 0;
}

