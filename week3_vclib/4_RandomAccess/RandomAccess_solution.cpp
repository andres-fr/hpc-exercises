  /// Gather and scatter values from given indices in an array 
  ///
  /// @authors: I.Kulakov; M.Zyzak
  /// @e-mail I.Kulakov@gsi.de; M.Zyzak@gsi.de
  /// 
  /// use "g++ RandomAccess_solution.cpp -O3 -lVc; ./a.out" to run

#include <Vc/Vc>
using namespace Vc;

#include <iostream>
using namespace std;

#include <cstdio>
#include <stdlib.h> // rand

const int N = 100; // matrix size. Has to be dividable by 4.

float input[N];
float output[N];

int main() {

  unsigned int index[float_v::Size];

  srand( time(NULL) ); // initialize random, so it will start from different numbers
  
  // fill input array with random numbers from 0 to 1
  for( int i = 0; i < N; i++ ) {
    input[i] = float(rand())/float(RAND_MAX);
  } 
  
  // fill output array with 0
  for( int i = 0; i < N; i++ ) {
    output[i] = 0;
  } 
  
  // fill indices with random numbers from 0 to N-1
  for( int i = 0; i < float_v::Size; i++) {
    index[i] = static_cast<unsigned int>(float(rand())/float(RAND_MAX)*N);
  }

  cout << "Indices: ";
  for(int i=0; i<float_v::Size; i++)
    cout << index[i] << " ";
  cout << endl;
  
  float_v tmp;
    /// gather without masking
  //TODO gather data with indices "index" from the array "input" into float_v tmp
  // Use void  gather (const float *array, const uint_v &indexes)

  uint_v ind(index);
  tmp.gather(input,ind);
  //check results
  bool ok = 1;
  for(int i=0; i<float_v::Size; i++) {
    if( fabs(tmp[i] - input[index[i]]) > 1.e-7) ok=0;
  }
  cout << "Gather without masking: results are ";
  if(ok) cout << "correct." << endl;
  else   cout << "WRONG." << endl;

    /// gather with masking
  //TODO gather data with indices "index" from the array "input" into float_v tmp2, if the value of "input" large then 0.5
  // Use void  gather (const float *array, const uint_v &indexes, const float_m &mask)
  float_m mask = tmp > 0.5f;
  float_v tmp2(Vc::Zero);
  tmp2.gather(input, ind, mask);
  //check results
  ok = 1;
  for(int i=0; i<float_v::Size; i++) {
    if( input[index[i]] > 0.5f ) {
      if( fabs(tmp2[i] - input[index[i]]) > 1.e-7) {
        cout << tmp2[i] << " is not equal to " << input[index[i]] << endl;
        ok=0;
      }
    }
    else {
      if( fabs(tmp2[i]) > 1.e-7) {
        cout << tmp2[i] << " is not equal to " << 0 << endl;
        ok=0;
      }
    }
  }
  cout << "Gather with masking: results are ";
  if(ok) cout << "correct." << endl;
  else   cout << "WRONG." << endl;
  
  //TODO create mask for values for an obtained tmp values, which are large than 0.5 and
  //TODO put all values smaller than 0.5 from tmp to the array "output" at the places given by indices "index"
  // Use void scatter (float *array, const uint_v &indexes, const float_m &mask) const
  mask = tmp < 0.5f;
  tmp.scatter(output, ind, mask);
    //check results
  ok = 1;
  for(int i=0; i<float_v::Size; i++) {
    if( tmp[i] < 0.5f ) {
      if( fabs(tmp[i] - output[index[i]]) > 1.e-7) {
        cout << tmp[i] << " is not equal to " << output[index[i]] << endl;
        ok=0;
      }
    }
    else {
      if( fabs(output[index[i]]) > 1.e-7) {
        cout << output[index[i]] << " is not equal to " << 0 << endl;
        ok=0;
      }
    }
  }
  cout << "Scatter with masking: results are ";
  if(ok) cout << "correct." << endl;
  else   cout << "WRONG." << endl;
  
  return 1;
}
