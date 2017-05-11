/* ==================================================
   Exersice. Pointers.
   Authors: I.Kulakov; M.Zyzak
   based on course of Jochen Gerhard
   ================================================== */

#include<iostream>
#include<cmath>
#include "xmmintrin.h" // for _mm_malloc
using namespace std;

int main ( int argc, char** argv )
{
  // When the size of the array is known
  const int SIZE = 10;
  float Array_static[SIZE];
  int i;
  for ( i = 0; i < SIZE; ++i)
  {
    Array_static[i] = sin(i) * 10.f + i ;
  }

  // When the size is unknown
  int size;
  cout << "Size of Array_dynamic and Array_dynamic_aligned: ";
  cin >> size; cout << endl;

  float* Array_dynamic = new float[size];
  // The access to the elements of this array is the same as for the usuall array.
  for ( i = 0; i < size; ++i )
  {
    Array_dynamic[i] = sin(i) * 10.f + i;
  }

  float* Array_dynamic_aligned = (float*) _mm_malloc(sizeof(float)*size,16*16*16);
  for ( i = 0; i < size; ++i )
  {
    Array_dynamic_aligned[i] = sin(i) * 10.f + i;
  }
  
  // Print the Array_static array to the screen
  std::cout << "Array_static ";
  for(i=0; i<SIZE; ++i)
    cout << Array_static[i] << " ";
  cout << endl;  

  // Print the Array_dynamic array to the screen
  std::cout << "Array_dynamic ";
  for(float *p = Array_dynamic; p < Array_dynamic + size; ++p)
    cout << *p << " ";
  cout << endl;

    // Print the Array_dynamic_aligned array to the screen
  std::cout << "Array_dynamic_aligned ";
  for(i=0; i<size; ++i)
    cout << Array_dynamic_aligned[i] << " ";
  cout << endl;  
  
  // 
  cout << endl << "*Array_dynamic:   " << *Array_dynamic << endl;
  *Array_dynamic = 42.3f;
  cout << "*Array_dynamic:   " << *Array_dynamic << endl;
  // And the *Array_dynamic really points to the first element:
  cout << "Array_dynamic[0]: " << Array_dynamic[0] << endl;

  cout << "Position in memory: " << endl << Array_static << endl << Array_dynamic << endl << Array_dynamic_aligned << endl;
  delete[] Array_dynamic;
  _mm_free(Array_dynamic_aligned);
  return 0;
}
