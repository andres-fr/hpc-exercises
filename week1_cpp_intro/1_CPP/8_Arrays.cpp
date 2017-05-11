/* ==================================================
   Exersice. Pointers.
   Authors: I.Kulakov; M.Zyzak
   ================================================== */

  // Run and check the output. Explain it.

#include<iostream>
using namespace std;

int main()
{
  const int N = 10;

    // declare pointers
  int* p1, *p2;

    // allocate memory
  p1 = new int[N];
  //int* pp = new int[123];
  p2 = new int[N];
  
    // fill
  for( int i = 0; i < N; ++i ) {
    p1[i] = i;
    p2[i] = i;
  }

    // change some values. Will arrays change?
  p1[11] = 1011;
  p1[15] = 1015;
  p2[13] = 2013;

    // print
  for( int i = 0; i < N; ++i ) {
    cout << "p1[" << i << "] = " << p1[i] << " " << "p2[" << i << "] = " << p2[i] << endl;
  }

  delete[] p1;
  delete[] p2;
}

