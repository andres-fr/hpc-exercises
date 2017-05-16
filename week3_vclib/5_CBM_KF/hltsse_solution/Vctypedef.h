#ifndef Vctypedef_H_
#define Vctypedef_H

#include <Vc/Vc>
using namespace Vc;
typedef float_v Fvec_t;
const int vecN = float_v::Size;

istream & operator>>(istream &strm, Fvec_t &a ){
  float tmp;
  strm>>tmp;
  a = tmp;
  return strm;
}

inline Fvec_t rcp(const Fvec_t &a) {
  return reciprocal(a);
}

#endif

