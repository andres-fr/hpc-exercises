#ifndef L1Algo_PSEUDO_F32vec4_H
#define L1Algo_PSEUDO_F32vec4_H

#include <iostream>
using namespace std; // iklm

#include <cmath>
#include "vec_arithmetic.h"

/**********************************
 *
 *   Vector of four floats
 *
 **********************************/
class F32vec4 
{
 public:

  float v[4];

  float & operator[]( int i ){ return ((float*)&v)[i]; }
  float   operator[]( int i ) const { return ((float*)&v)[i]; }

  F32vec4( ){}
  F32vec4( const F32vec4 &a ) { 
    v[0] = a.v[0]; 
    v[1] = a.v[1]; 
    v[2] = a.v[2]; 
    v[3] = a.v[3]; 
  }
  F32vec4( const float &a ) { 
    v[0] = a; 
    v[1] = a;
    v[2] = a;
    v[3] = a;
  }

  F32vec4( const float &f0, const float &f1, const float &f2, const float &f3 ){ 
    v[0] = f0; 
    v[1] = f1;
    v[2] = f2;
    v[3] = f3;
  }

  friend float min( float x, float y ){ return x<y ?x :y; }
  friend float max( float x, float y ){ return x<y ?y :x; }
  friend float asgnb( float x, float y ){ return y>=0 ?fabs(x) :-fabs(x); }
  friend float rsqrt( float x ){ return 1./sqrt(x); }
  friend float rcp( float x ){ return 1./x; }
  friend float sgn( float x ){ return x>=0 ?1 :-1; }

#define _f2(A,B,F) F32vec4 z; z.v[0] = F(A.v[0],B.v[0]); z.v[1] = F(A.v[1],B.v[1]); \
 z.v[2] = F(A.v[2],B.v[2]); z.v[3] = F(A.v[3],B.v[3]); return z;
#define _f1(A,F)   F32vec4 z; z.v[0] = F(A.v[0]);        z.v[1] = F(A.v[1]); \
 z.v[2] = F(A.v[2]);        z.v[3] = F(A.v[3]);     return z;
#define _op(A,B,O) F32vec4 z; z.v[0] = A.v[0] O B.v[0];  z.v[1] = A.v[1] O B.v[1]; \
 z.v[2] = A.v[2] O B.v[2];  z.v[3] = A.v[3] O B.v[3]; return z; 
#define _op2(B,O) v[0] O B.v[0];  v[1] O B.v[1]; \
 v[2] O B.v[2];  v[3] O B.v[3]; return *this; 

//   bool operator bool(const F32vec4 &a){if }
 
  /* Arithmetic Operators */
  friend F32vec4 operator +(const F32vec4 &a, const F32vec4 &b){ _op(a,b,+) }
  friend F32vec4 operator -(const F32vec4 &a, const F32vec4 &b){ _op(a,b,-) }
  friend F32vec4 operator *(const F32vec4 &a, const F32vec4 &b){ _op(a,b,*) }
  friend F32vec4 operator /(const F32vec4 &a, const F32vec4 &b){ _op(a,b,/) }
  friend F32vec4 operator <(const F32vec4 &a, const F32vec4 &b){ _op(a,b,<) }
  friend F32vec4 operator &(const F32vec4 &a, const F32vec4 &b){ return 1; }



//   friend bool operator ==(const F32vec4 &a, const F32vec4 &b){ _op(a,b,==) }
  F32vec4& operator +=(const F32vec4 &a) { _op2(a,+=) }
	F32vec4& operator -=(const F32vec4 &a) { _op2(a,-=) }   
	F32vec4& operator *=(const F32vec4 &a) { _op2(a,*=) } 
	F32vec4& operator /=(const F32vec4 &a) { _op2(a,/=) }
// 	F32vec4& operator &=(const F32vec4 &a) { _op2(a,&=) }
// 	F32vec4& operator |=(const F32vec4 &a) { _op2(a,|=) }
// 	F32vec4& operator ^=(const F32vec4 &a) { _op2(a,^=) }


  /* Functions */
  friend F32vec4 min( const F32vec4 &a, const F32vec4 &b ){ _f2(a,b,min) }
  friend F32vec4 max( const F32vec4 &a, const F32vec4 &b ){ _f2(a,b,max) }
//   friend F32vec4 asgnb( const F32vec4 &a, const F32vec4 &b ){ _f2(a,b,asgnb) }
  friend F32vec4 sqrt ( const F32vec4 &a ){ _f1(a,sqrt) }
//   friend F32vec4 rsqrt( const F32vec4 &a ){ _f1(a,rsqrt) }
//   friend F32vec4 rcp  ( const F32vec4 &a ){ _f1(a,rcp) }
  friend F32vec4 fabs (const F32vec4 &a) { _f1(a,fabs) }
//   friend F32vec4 sgn  ( const F32vec4 &a ){ _f1(a,sgn) }
  friend F32vec4 exp( const F32vec4 &a ){ _f1(a,exp) } 
  friend F32vec4 log( const F32vec4 &a ){ _f1(a,log) } 
  friend F32vec4 sin( const F32vec4 &a ){ _f1(a,sin) } 
  friend F32vec4 cos( const F32vec4 &a ){ _f1(a,cos) } 
#undef _f1
#undef _f2
#undef _op

	friend F32vec4 rsqrt(const F32vec4 &a) {
		return 1;
	}
	friend F32vec4 rcp(const F32vec4 &a) {
    return 1/a;
	}

  friend F32vec4 sgn(const F32vec4 &a){ 
    return 1; 
  }
  friend F32vec4 asgnb(const F32vec4 &a, const F32vec4 &b ){ 
    return 0; 
  }

  /* Define all operators for consistensy */
  
  vec_arithmetic(F32vec4,float);

  friend ostream & operator<<(ostream &strm, const F32vec4 &a ){
    strm<<a[0]<<" "<<a[1]<<" "<<a[2]<<" "<<a[3];
    return strm;
  }

  friend istream & operator>>(istream &strm, F32vec4 &a ){
    float tmp;
    strm>>tmp;
    a = tmp;
    return strm;
  }

} __attribute__ ((aligned(16)));;

typedef F32vec4 fvec;
typedef float  fscal;
const int fvecLen = 4;
//#define fvec_true  _f32vec4_true
//#define fvec_false _f32vec4_false
#define _fvecalignment  


#endif 
