/* ==================================================
   Exersice. Pointers.
   Authors: I.Kulakov; M.Zyzak
   ================================================== */

// Will this program work? Find 2 bugs. Fix them with minimum changes in the program code.

#include<iostream>
using namespace std;

const int N = 10;

// Get set of the factorials of the first N numbers
int* GetFactorials(){
  int* a = new int[N]; // dynamic instead of static

  a[0] = 1;
  for( int i = 1; i < N; ++i )
    a[i] = i*a[i-1];

  return a;
}

int main() {
  // Get set of the factorials of the first N numbers
  int* a = GetFactorials();
 
  // print it
  for( int i = 0; i < N; ++i )
    cout << a[i] << endl;

  delete[] a;
  
  return 0;
}

