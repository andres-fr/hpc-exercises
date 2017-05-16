/// The maximum root of a quadratic equation a*x^2+b*x+c = 0
///
/// @author I.Kulakov, M.Zyzak
/// @e-mail I.Kulakov@gsi.de, M.Zyzak@gsi.de
///
/// use "g++ QuadraticEquation_solution.cpp -O3 -fno-tree-vectorize -g -msse -lVc; ./a.out" to run

#include <Vc/Vc>
using namespace Vc;

#include <cmath>
#include <iostream>
using namespace std;

#include <stdlib.h> // rand
#include "../../../TStopwatch.h"

static const int NVectors = 100000;
static const int N = NVectors*float_v::Size;

static const int NIterOut = 100;

struct DataAOSElement {
  float a, b, c, // coefficients
    x; // a root
};

struct DataAOS {
  DataAOS(const int N) {
    data = (DataAOSElement*) _mm_malloc(sizeof(DataAOSElement)*N,16);
  }
  ~DataAOS() {
    if(data) _mm_free(data);
  }
  DataAOSElement *data;
};

struct DataSOA {
  
  DataSOA(const int N) {
    a = (float*) _mm_malloc(sizeof(float)*N,16);
    b = (float*) _mm_malloc(sizeof(float)*N,16);
    c = (float*) _mm_malloc(sizeof(float)*N,16);
    x = (float*) _mm_malloc(sizeof(float)*N,16);
  }
  ~DataSOA()
  {
    if(a) _mm_free(a);
    if(b) _mm_free(b);
    if(c) _mm_free(c);
    if(x) _mm_free(x);
  }
  
  float *a, *b, *c, // coefficients
    *x; // a root
};

struct DataAOSOAElement {
  
  void SetMemory(float *m) {
    a = m;
    b = m + float_v::Size;
    c = m + 2*float_v::Size;
    x = m + 3*float_v::Size;
  }
  
  float *a, *b, *c, // coefficients
    *x; // a root
};

struct DataAOSOA {
  DataAOSOA(const int N) {
    const int NVectors = N/float_v::Size;
    
    data = new DataAOSOAElement[NVectors];
    memory = (float*) _mm_malloc(sizeof(float)*4*N,16);
    
    float *memp = memory;
    for( int i = 0; i < NVectors; i++ ) {
      data[i].SetMemory(memp);
      memp += float_v::Size*4;
    }
  }
  ~DataAOSOA() {
    _mm_free(memory);
    delete[] data;
  }
  
  float *memory;
  DataAOSOAElement *data;
};

void CheckResults(const DataAOS& yScalar, const DataAOS& ySIMD, const int NSIMD)
{
  bool ok = 1;
  for(int i=0; i<N; i++)
    if( fabs(yScalar.data[i].x - ySIMD.data[i].x) > yScalar.data[i].x*0.01 )
    {
      ok = 0;
      // std::cout << i<<" " << yScalar.data[i].x << " " << ySIMD.data[i].x << " " << fabs(yScalar.data[i].x - ySIMD.data[i].x)<<std::endl;
    }
  if(!ok)
    std::cout << "ERROR! SIMD and scalar results are not the same. SIMD part " << NSIMD << "." << std::endl;
  else
    std::cout << "SIMD " << NSIMD << " and scalar results are the same." << std::endl;
}

void CheckResults(const DataAOS& yScalar, const DataSOA& ySIMD, const int NSIMD)
{
  bool ok = 1;
  for(int i=0; i<N; i++)
    if( fabs(yScalar.data[i].x - ySIMD.x[i]) > yScalar.data[i].x*0.01 )
    {
      ok = 0;
      // std::cout << i<<" " << yScalar.data[i].x << " " << ySIMD.x[i] << " " << fabs(yScalar.data[i].x - ySIMD.x[i])<<std::endl;
    }
  if(!ok)
    std::cout << "ERROR! SIMD and scalar results are not the same. SIMD part " << NSIMD << "." << std::endl;
  else
    std::cout << "SIMD " << NSIMD << " and scalar results are the same." << std::endl;
}

void CheckResults(const DataAOS& yScalar, const DataAOSOA& ySIMD, const int NSIMD)
{
  bool ok = 1;
  for(int i=0; i<N; i++) {
    const int nV = i/float_v::Size;
    const int iV = i%float_v::Size;
    if( fabs(yScalar.data[i].x - ySIMD.data[nV].x[iV]) > yScalar.data[i].x*0.01 )
    {
      ok = 0;
      // std::cout << i<<" " << yScalar.data[i].x << " " << ySIMD.data[nV].x[iV] << " " << fabs(yScalar.data[i].x - ySIMD.data[nV].x[iV])<<std::endl;
    }
  }
  if(!ok)
    std::cout << "ERROR! SIMD and scalar results are not the same. SIMD part " << NSIMD << "." << std::endl;
  else
    std::cout << "SIMD " << NSIMD << " and scalar results are the same." << std::endl;
}

int main() {
  
  //input data
  
  DataAOS dataScalar(N);
  DataAOS dataSIMD1(N);
  DataSOA dataSIMD2(N);
  DataAOSOA dataSIMD3(N);
  
  // fill parameters by random numbers
  for( int i = 0; i < N; i++ ) {
    float a = 0.01 + float(rand())/float(RAND_MAX); // put a random value, from 0.01 to 1.01 (a has not to be equal 0)
    float b = float(rand())/float(RAND_MAX);
    float c = -float(rand())/float(RAND_MAX);
    
    dataScalar.data[i].a = a;
    dataScalar.data[i].b = b;
    dataScalar.data[i].c = c;
    
    dataSIMD1.data[i].a = a;
    dataSIMD1.data[i].b = b;
    dataSIMD1.data[i].c = c;
    
    dataSIMD2.a[i] = a;
    dataSIMD2.b[i] = b;
    dataSIMD2.c[i] = c;
    
    const int nV = i/float_v::Size;
    const int iV = i%float_v::Size;
    dataSIMD3.data[nV].a[iV] = a;
    dataSIMD3.data[nV].b[iV] = b;
    dataSIMD3.data[nV].c[iV] = c;
  }
  
  /// -- CALCULATE --
  
  // scalar calculations
  TStopwatch timerScalar;
  for(int io=0; io<NIterOut; io++)
    for(int i=0; i<N; i++)
    {
      float &a = dataScalar.data[i].a;
      float &b = dataScalar.data[i].b;
      float &c = dataScalar.data[i].c;
      float &x = dataScalar.data[i].x;
      
      float det = b*b - 4*a*c;
      x = (-b+sqrt(det))/(2*a);
    }
  timerScalar.Stop();
  
  // SIMD calculations with Vc using dataSIMD1 as input
  TStopwatch timerSIMD1;
  for(int io=0; io<NIterOut; io++)
  {
    for(int i=0; i<NVectors; i++)
    {
      // TODO
    }
  }
  timerSIMD1.Stop();
  
  // SIMD calculations with Vc using dataSIMD2 as input (use cast here)
  TStopwatch timerSIMD2;
  for(int io=0; io<NIterOut; io++)
    for(int i=0; i<N; i+=float_v::Size)
    {
      // TODO
    }
  timerSIMD2.Stop();
  
  // SIMD calculations with Vc using dataSIMD2 as input (use cast here)
  TStopwatch timerSIMD3;
  for(int io=0; io<NIterOut; io++)
    for(int i=0; i<NVectors; i++)
    {
      // TODO
    }
  timerSIMD3.Stop();
  
  double tScal = timerScalar.RealTime()*1000;
  double tSIMD1 = timerSIMD1.RealTime()*1000;
  double tSIMD2 = timerSIMD2.RealTime()*1000;
  double tSIMD3 = timerSIMD3.RealTime()*1000;
  cout << "Time scalar:   " << tScal << " ms " << endl;
  cout << "Time Vc AOS:   " << tSIMD1 << " ms, speed up " << tScal/tSIMD1 << endl;
  cout << "Time Vc SOA:   " << tSIMD2 << " ms, speed up " << tScal/tSIMD2 << endl;
  cout << "Time Vc AOSOA: " << tSIMD3 << " ms, speed up " << tScal/tSIMD3 << endl;
  
  //compare SIMD and scalar results
  CheckResults(dataScalar,dataSIMD1,1);
  CheckResults(dataScalar,dataSIMD2,2);
  CheckResults(dataScalar,dataSIMD3,3);
  
  return 1;
}
