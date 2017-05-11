/* ==================================================
   Exersice. Pointers.
   Authors: I.Kulakov; M.Zyzak
   ================================================== */

// Will this program work? Run and explane the output. Find 12 places where pointers are used incorrectly. Which ones do lead to crash? Explain the behavior of the program when you change k from 1 to 3.

#include <iostream>
using namespace std;

const short k = 3;

int main ( )
{
  float* p = new float(123); // allocate the memory, initialize with 123
  float* p1 = p;
  p1++;

  cout << " PART 1 " << endl;
  cout << "Initial value:        " << p << "  " << *p << endl;
  (*p)++; //increase the value
  cout << "Increased value:      " << p << "  " << *p << endl;

    //clean the memory;
  cout << " PART 2 " << endl;
  cout << "Address p before delete: " << p << "  " << *p << endl;  
  delete p;
  cout << "Address p after delete:  " << p << "  " << *p << endl; // memory error (valgrind)

  if(k == 2) {
    cout << " PART 3 " << endl;
    cout << "Address p before delete: " << p << "  " << *p << endl;  
    delete p; //free the memory
    cout << "Address p after delete:  " << p << "  " << *p << endl; // memory error (valgrind)
  }
  if(k >= 2) {
    cout << " PART 4 " << endl;
    cout << "Address p1 before delete: " << p1 << "  " << *p1 << endl;    
    delete p1; //free the memory
    cout << "Address p1 after delete:  " << p1 << "  " << *p1 << endl;  
  }

  return 0;
}
