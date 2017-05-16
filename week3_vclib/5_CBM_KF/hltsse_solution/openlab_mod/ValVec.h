#pragma once

#include <iostream> 
#include <valarray>

using namespace std;

#include "../vec_arithmetic.h"

/*
 * @author: Ralf Ratering
 * @date: 6. March 2008
 */
class ValVec {
private:
	valarray<float> v;

	ValVec(const valarray<float> &x) : v(x) {}

	// convert from bool array
	ValVec(const valarray<bool> &x) : v(VEC_SIZE) {
		for(size_t i=0; i<v.size(); i++) {
			if(x[i]) {
				v[i]=1.0f;
			}
			else {
				v[i]=0.0f;
			}
		}
	}

public:
	// SET SIZE HERE!
	const static int VEC_SIZE = 128;

	ValVec() : v(0.0f, VEC_SIZE) {}

	// default constructor sets all values to zero
	ValVec(float val) : v(val, VEC_SIZE) {}

	~ValVec(void) {}
	
	float & operator[]( int i ){ return v[i]; }
	float   operator[]( int i ) const { return v[i]; }
	
	ValVec apply(float func(const float&)) {
		return ValVec(v.apply(func));
	}

	friend ValVec operator+(const ValVec& a, const ValVec &b) { return ValVec(a.v + b.v); }
	friend ValVec operator-(const ValVec& a, const ValVec &b) { return ValVec(a.v - b.v); }
	friend ValVec operator*(const ValVec& a, const ValVec &b) { return ValVec(a.v * b.v); }
	friend ValVec operator/(const ValVec& a, const ValVec &b) { return ValVec(a.v / b.v); }
	
	// @CAUTION: replaced bitwise AND with logical AND 
	friend ValVec operator&(const ValVec& a, const ValVec &b) { return ValVec(a.v && b.v); }
	
	friend ValVec operator> (const ValVec& a, const ValVec &b) { return ValVec(a.v > b.v); }
	friend ValVec operator< (const ValVec& a, const ValVec &b) { return ValVec(a.v < b.v); }
	friend ValVec operator==(const ValVec& a, const ValVec &b) { return ValVec(a.v == b.v); }
	friend ValVec operator!=(const ValVec& a, const ValVec &b) { return ValVec(a.v != b.v); }
	
	friend ValVec log ( const ValVec &a ) { return ValVec(log(a.v)); }
	
	friend ValVec rcp ( const ValVec &a ) { return 1.0f/a.v; }
	friend ValVec sqrt ( const ValVec &a ) { return ValVec(sqrt(a.v)); }
	friend ValVec rsqrt ( const ValVec &a ) { return rcp(sqrt(a.v)); }

    // This will prevent temp vectors from being allocated. 
    ValVec& operator +=(const ValVec &a) { v+=a.v; return *this; }
	ValVec& operator -=(const ValVec &a) { v-=a.v; return *this; }
	ValVec& operator *=(const ValVec &a) { v*=a.v; return *this; }
	ValVec& operator /=(const ValVec &a) { v/=a.v; return *this; }
	
	// call define from include file for other ops
    vec_arithmetic(ValVec, float);

	template<typename _CharT, class _Traits> 
	  friend basic_ostream<_CharT,_Traits>& 
	  operator<<( basic_ostream<_CharT,_Traits> & out, ValVec& vec) {
		out << "(";
		for(size_t i=0; i<vec.v.size()-1; i++) {
			out << vec.v[i] << ", ";
		}
		out << vec.v[vec.v.size()-1] << ")";
		return out;
	}

	friend istream & operator>>(istream &strm, ValVec &a ){
	    float tmp;
		strm>>tmp;
		a = tmp;
		return strm;
	}
};
