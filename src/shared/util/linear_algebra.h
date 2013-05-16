#pragma once


#include <iostream>
#include <cmath>


namespace linear_algebra {
	
	
	// Totally generic
	template <int N, class R>
	struct vec {
		static vec origin ();
	
		R a[N];
	};
	
	// 2D specialization
	template <class R>
	struct vec<2,R> {
		vec () {};
		vec (R x, R y) : x(x), y(y) {}
		
		static vec origin () { return vec(0,0); }
		static vec x_unit () { return vec(1,0); }
		static vec y_unit () { return vec(0,1); }
	
		union {
			R a[2];
			struct {
				R x;
				R y;
			};
		};
	};
	
	// 3D specialization
	template <class R>
	struct vec<3,R> {
		vec () {};
		vec (R x, R y, R z) : x(x), y(y), z(z) {}
		
		static vec origin () { return vec(0,0,0); }
		static vec x_unit () { return vec(1,0,0); }
		static vec y_unit () { return vec(0,1,0); }
		static vec z_unit () { return vec(0,0,1); }
	
		union {
			R a[3];
			struct {
				R x;
				R y;
				R z;
			};
		};
	};
	
	// Constant /////////////////////////////////////////////////////////////////////////////////
	template <int N, class R>
	vec<N,R> vec<N,R>::origin () {
		vec<N,R> v;
		for (int i=0; i<N; i++) v.a[i] = 0;
		return v;
	}
	
	// Vector addition ////////////////////////////////////////////////////////////////////
	template <int N, class R>
	vec<N,R> operator +  (const vec<N,R>& lhs, const vec<N,R>& rhs) {
		vec<N,float> v;
		for (int i=0; i<N; i++) v.a[i] = lhs.a[i] + rhs.a[i];
		return v;
	}
	template <int N, class R>
	vec<N,R> operator -  (const vec<N,R>& lhs, const vec<N,R>& rhs) {
		vec<N,float> v;
		for (int i=0; i<N; i++) v.a[i] = lhs.a[i] - rhs.a[i];
		return v;
	}
	template <int N, class R>
	vec<N,R> operator += (vec<N,R>& lhs, const vec<N,R>& rhs) {
		for (int i=0; i<N; i++) lhs.a[i] += rhs.a[i];
		return lhs;
	}
	template <int N, class R>
	vec<N,R> operator -= (vec<N,R>& lhs, const vec<N,R>& rhs) {
		for (int i=0; i<N; i++) lhs.a[i] -= rhs.a[i];
		return lhs;
	}
	
	// Vector-vector multiplication /////////////////////////////////////////////////////////////////
	template <int N, class R>
	vec<N,R> operator *  (const vec<N,R>& lhs, const vec<N,R>& rhs) {
		vec<N,R> v;
		for (int i=0; i<N; i++) v.a[i] = lhs.a[i] * rhs.a[i];
		return v;
	}
	template <int N, class R>
	vec<N,R> operator /  (const vec<N,R>& lhs, const vec<N,R>& rhs) {
		vec<N,R> v;
		for (int i=0; i<N; i++) v.a[i] = lhs.a[i] / rhs.a[i];
		return v;
	}
	template <int N, class R>
	vec<N,R> operator *= (vec<N,R>& lhs, const vec<N,R>& rhs) {
		for (int i=0; i<N; i++) lhs.a[i] *= rhs.a[i];
	}
	template <int N, class R>
	vec<N,R> operator /= (vec<N,R>& lhs, const vec<N,R>& rhs) {
		for (int i=0; i<N; i++) lhs.a[i] /= rhs.a[i];
	}
	
	// Vector-scalar multiplication /////////////////////////////////////////////////////////////////
	template <int N, class R>
	vec<N,R> operator *  (const vec<N,R>& lhs, const R& rhs) {
		vec<N,R> v;
		for (int i=0; i<N; i++) v.a[i] = lhs.a[i] * rhs;
		return v;
	}
	template <int N, class R>
	vec<N,R> operator /  (const vec<N,R>& lhs, const R& rhs) {
		vec<N,R> v;
		for (int i=0; i<N; i++) v.a[i] = lhs.a[i] / rhs;
		return v;
	}
	template <int N, class R>
	vec<N,R> operator *= (vec<N,R>& lhs, const R& rhs) {
		for (int i=0; i<N; i++) lhs.a[i] *= rhs;
		return lhs;
	}
	template <int N, class R>
	vec<N,R> operator /= (vec<N,R>& lhs, const R& rhs) {
		for (int i=0; i<N; i++) lhs.a[i] /= rhs;
		return lhs;
	}
	
	// Vector-scalar division optimization for float types //////////////////////////////////
	template <int N>
	vec<N,float> operator /  (const vec<N,float>& lhs, const float& rhs) {
		float irhs = 1.0f/rhs;
		vec<N,float> v;
		for (int i=0; i<N; i++) v.a[i] = lhs.a[i]*irhs;
		return v;
	}
	template <int N>
	vec<N,double> operator /  (const vec<N,double>& lhs, const double& rhs) {
		double irhs = 1.0/rhs;
		vec<N,double> v;
		for (int i=0; i<N; i++) v.a[i] = lhs.a[i]*irhs;
		return v;
	}
	
	// Comparisons //////////////////////////////////////////////////////////////////////////////
	template <int N, class R> bool operator <  (const vec<N,R>& lhs, const vec<N,R>& rhs) { return magsquared(lhs) <  magsquared(rhs); }
	template <int N, class R> bool operator <= (const vec<N,R>& lhs, const vec<N,R>& rhs) { return magsquared(lhs) <= magsquared(rhs); }
	template <int N, class R> bool operator >  (const vec<N,R>& lhs, const vec<N,R>& rhs) { return magsquared(lhs) >  magsquared(rhs); }
	template <int N, class R> bool operator >= (const vec<N,R>& lhs, const vec<N,R>& rhs) { return magsquared(lhs) >= magsquared(rhs); }
	template <int N, class R> bool operator == (const vec<N,R>& lhs, const vec<N,R>& rhs) {
		for (int i=0; i<N; i++) if (lhs.a[i]!=rhs.a[i]) return false;
		return true;
	}
	template <int N, class R> bool operator != (const vec<N,R>& lhs, const vec<N,R>& rhs) {
		for (int i=0; i<N; i++) if (lhs.a[i]!=rhs.a[i]) return true;
		return false;
	}
	
	// Stream insertion /////////////////////////////////////////////////////////////////////////
	template <int N, class R>
	std::ostream& operator<< (std::ostream& os, const vec<N,R>& v) {
		os << '(' << v.a[0];
		for (int i=1; i<N; i++)
			os << ", " << v.a[i];
		os << ')';
		return os;
	}
	
	// Utility functions /////////////////////////////////////////////////////////////////////////
	template <int N, class R> R        magsquared (const vec<N,R>& v) {
		R sum = 0;
		for (int i=0; i<N; i++) sum += v.a[i]*v.a[i];
		return sum;
	}
	template <int N, class R> R        mag        (const vec<N,R>& v) { return std::sqrt(magsquared(v)); }
	template <int N, class R> vec<N,R> unit       (const vec<N,R>& v) { return v / mag(v); }
	template <int N, class R> void     normalize  (vec<N,R>& v)       { v /= mag(v); }
	template <int N, class R> R        dot        (const vec<N,R>& v1, const vec<N,R>& v2) {
		R sum = 0;
		for (int i=0; i<N; i++) sum += v1.a[i]*v2.a[i];
		return sum;
	}
	template <int N, class R> float    angle      (const vec<N,R>& v1, const vec<N,R>& v2) {
		return std::acos(dot(unit(v1), unit(v2)));
	}
	// 3D only
	template <class R>        vec<3,R> cross      (const vec<3,R>& v1, const vec<3,R>& v2) {
		return vec<3,R>(
			v1.y*v2.z - v1.z*v2.y,
			v1.z*v2.x - v1.x*v2.z,
			v1.x*v2.y - v1.y*v2.x
		);
	}
	

}
