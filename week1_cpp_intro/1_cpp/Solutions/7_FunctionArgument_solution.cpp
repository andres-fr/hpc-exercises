/* ==================================================
   Exersice. Pointers.
   Authors: I.Kulakov; M.Zyzak
   ================================================== */

// Finish the program (see the comments in main function)

#include<iostream>
using namespace std;
  
void increase1(int arg)
{
  arg++;
}
  
int increase2(int arg)
{
  arg++;
  return arg;
}
  
void pointer_increase(int* arg)
{
  (*arg)++;
}

void reference_increase(int& arg)
{
  arg++;
}

int main ()
{
  int number = 0;
  cout << "Number is: " << number  << endl;
  increase1( number ); // Has no effect.
  cout << "Number is: " << number  << endl;
  number = increase2( number );  // increase number by 1.
  cout << "Number is: " << number << endl;
  pointer_increase( &number ); // increase number by 1.
  cout << "Number is: " << number << endl;
  reference_increase( number ); // increase number by 1.
  cout << "Number is: " << number << endl;
  return 0;
}

