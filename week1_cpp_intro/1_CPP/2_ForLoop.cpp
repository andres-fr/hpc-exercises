/* ==================================================
   Exersice. For loop.
   Authors: I.Kulakov; M.Zyzak
   ================================================== */

  // Run and understand 


  // countdown using a for loop
#include <iostream>
using namespace std;
int main()
{
  float* test = new float[10];
  for (int n=10; n > 0; n--) {
    cout << n << ", ";
    cout << ">>" << test[n-1] << endl;

    if ( n == 1 )                                    
      cout << "FIRE!" << endl;
  }
  return 0;
}
