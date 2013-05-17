

/*
	Higher Math Utility Headers
	http://github.com/dustyco/hmath
	
	Linear Algebra
	See github wiki for usage info
	https://github.com/dustyco/hmath/wiki/Linear-Algebra
	
	Changes:
		2013-03-07
			Switched vector-scalar division overloads to use std::numeric_limits<R>::is_iec559 instead
			Added vector bracket operator
*/


#pragma once
#include <algorithm>
#include <iostream>
#include <limits>
#include <cmath>


namespace hmath {
	
	// For static type comparison
	template <class T, class U>
	struct is_same { static const bool value = false; };
	template <class T>
	struct is_same<T, T> { static const bool value = true; };
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	// TYPES ///////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	// Totally generic
	template <int N, class R>
	struct vec {
		vec (const R input[N]) { std::copy(input, input+N, a); }
		static vec origin ();
		
		R&       operator[] (unsigned int i)       { return a[i]; }
		const R& operator[] (unsigned int i) const { return a[i]; }
	
		R a[N];
	};
	
	// 2D specialization
	template <class R>
	struct vec<2,R> {
		vec () {};
		vec (R x, R y) : x(x), y(y) {}
		vec (const R input[2]) { std::copy(input, input+2, a); }
		
		static vec origin () { return vec(0,0); }
		static vec x_unit () { return vec(1,0); }
		static vec y_unit () { return vec(0,1); }
		
		R&       operator[] (unsigned int i)       { return a[i]; }
		const R& operator[] (unsigned int i) const { return a[i]; }
	
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
		vec (const R input[3]) { std::copy(input, input+3, a); }
		
		static vec origin () { return vec(0,0,0); }
		static vec x_unit () { return vec(1,0,0); }
		static vec y_unit () { return vec(0,1,0); }
		static vec z_unit () { return vec(0,0,1); }
		
		R&       operator[] (unsigned int i)       { return a[i]; }
		const R& operator[] (unsigned int i) const { return a[i]; }
	
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
		for (int i=0; i<N; i++) v[i] = 0;
		return v;
	}
	
	// Vector addition ////////////////////////////////////////////////////////////////////
	template <int N, class R>
	vec<N,R> operator +  (const vec<N,R>& lhs, const vec<N,R>& rhs) {
		vec<N,float> v;
		for (int i=0; i<N; i++) v[i] = lhs[i] + rhs[i];
		return v;
	}
	template <int N, class R>
	vec<N,R> operator -  (const vec<N,R>& lhs, const vec<N,R>& rhs) {
		vec<N,float> v;
		for (int i=0; i<N; i++) v[i] = lhs[i] - rhs[i];
		return v;
	}
	template <int N, class R>
	vec<N,R> operator += (vec<N,R>& lhs, const vec<N,R>& rhs) {
		for (int i=0; i<N; i++) lhs[i] += rhs[i];
		return lhs;
	}
	template <int N, class R>
	vec<N,R> operator -= (vec<N,R>& lhs, const vec<N,R>& rhs) {
		for (int i=0; i<N; i++) lhs[i] -= rhs[i];
		return lhs;
	}
	
	// Vector-vector multiplication /////////////////////////////////////////////////////////////////
	template <int N, class R>
	vec<N,R> operator *  (const vec<N,R>& lhs, const vec<N,R>& rhs) {
		vec<N,R> v;
		for (int i=0; i<N; i++) v[i] = lhs[i] * rhs[i];
		return v;
	}
	template <int N, class R>
	vec<N,R> operator /  (const vec<N,R>& lhs, const vec<N,R>& rhs) {
		vec<N,R> v;
		for (int i=0; i<N; i++) v[i] = lhs[i] / rhs[i];
		return v;
	}
	template <int N, class R>
	vec<N,R> operator *= (vec<N,R>& lhs, const vec<N,R>& rhs) {
		for (int i=0; i<N; i++) lhs[i] *= rhs[i];
	}
	template <int N, class R>
	vec<N,R> operator /= (vec<N,R>& lhs, const vec<N,R>& rhs) {
		for (int i=0; i<N; i++) lhs[i] /= rhs[i];
	}
	
	// Vector-scalar multiplication /////////////////////////////////////////////////////////////////
	template <int N, class R>
	vec<N,R> operator *  (const vec<N,R>& lhs, const R& rhs) {
		vec<N,R> v;
		for (int i=0; i<N; i++) v[i] = lhs[i] * rhs;
		return v;
	}
	template <int N, class R>
	vec<N,R> operator /  (const vec<N,R>& lhs, const R& rhs) {
		vec<N,R> v;
		if (std::numeric_limits<R>::is_iec559 && N>1) {
			// Cache reciprocol to avoid multiple divisions
			R irhs = R(1)/rhs;
			for (int i=0; i<N; i++) v[i] = lhs[i]*irhs;
		} else {
			// Otherwise divide normally (integers or custom types)
			for (int i=0; i<N; i++) v[i] = lhs[i] / rhs;
		}
		return v;
	}
	template <int N, class R>
	vec<N,R> operator *= (vec<N,R>& lhs, const R& rhs) {
		for (int i=0; i<N; i++) lhs[i] *= rhs;
		return lhs;
	}
	template <int N, class R>
	vec<N,R> operator /= (vec<N,R>& lhs, const R& rhs) {
		if (std::numeric_limits<R>::is_iec559 && N>1) {
			// Cache reciprocol to avoid multiple divisions
			R irhs = R(1)/rhs;
			for (int i=0; i<N; i++) lhs[i] = lhs[i]*irhs;
		} else {
			// Otherwise divide normally (integers or custom types)
			for (int i=0; i<N; i++) lhs[i] /= rhs;
		}
		return lhs;
	}
	
	// Comparisons //////////////////////////////////////////////////////////////////////////////
	template <int N, class R> bool operator <  (const vec<N,R>& lhs, const vec<N,R>& rhs) { return magsquared(lhs) <  magsquared(rhs); }
	template <int N, class R> bool operator <= (const vec<N,R>& lhs, const vec<N,R>& rhs) { return magsquared(lhs) <= magsquared(rhs); }
	template <int N, class R> bool operator >  (const vec<N,R>& lhs, const vec<N,R>& rhs) { return magsquared(lhs) >  magsquared(rhs); }
	template <int N, class R> bool operator >= (const vec<N,R>& lhs, const vec<N,R>& rhs) { return magsquared(lhs) >= magsquared(rhs); }
	template <int N, class R> bool operator == (const vec<N,R>& lhs, const vec<N,R>& rhs) {
		for (int i=0; i<N; i++) if (lhs[i]!=rhs[i]) return false;
		return true;
	}
	template <int N, class R> bool operator != (const vec<N,R>& lhs, const vec<N,R>& rhs) {
		for (int i=0; i<N; i++) if (lhs[i]!=rhs[i]) return true;
		return false;
	}
	
	// Stream insertion /////////////////////////////////////////////////////////////////////////
	template <int N, class R>
	std::ostream& operator<< (std::ostream& os, const vec<N,R>& v) {
		os << '(' << v[0];
		for (int i=1; i<N; i++)
			os << ", " << v[i];
		os << ')';
		return os;
	}
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ALGORITHMS //////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	template <int N, class R>         R length_squared (const vec<N,R>& v) {
		R sum = 0;
		for (int i=0; i<N; i++) sum += v[i]*v[i];
		return sum;
	}
	template <int N, class R>        R  length     (const vec<N,R>& v) { return std::sqrt(length_squared(v)); }
	template <int N, class R> vec<N,R>  unit       (const vec<N,R>& v) { return v / length(v); }
	template <int N, class R>     void  normalize  (vec<N,R>& v)       { v /= length(v); }
	template <int N, class R>        R  dot        (const vec<N,R>& v1, const vec<N,R>& v2) {
		R sum = 0;
		for (int i=0; i<N; i++) sum += v1[i]*v2[i];
		return sum;
	}
	// 2D: Return the normal vector of a vector (rotate it by 90 degrees clockwise, same magnitude)
	template <class R>        vec<2,R>  normal     (const vec<2,R>& v) { return vec<2,R>(v.y, -v.x); }
	template <class R>        vec<2,R>  normal_i   (const vec<2,R>& v) { return vec<2,R>(-v.y, v.x); }
	
	// 2D only
	template <int N, class R>    float  angle      (const vec<N,R>& v1, const vec<N,R>& v2) {
		return std::acos(dot(unit(v1), unit(v2)));
	}
	// 2D only
	template <class R>           float  angle_aa   (const vec<2,R>& v) {
		if (v.x>0) {
			// QI or QIV
			if (v.y>0) {
				// QI
				return std::atan(float(v.y)/float(v.x));
			} else if (v.y<0) {
				// QIV
				return std::atan(float(v.x)/std::fabs(float(v.y))) + 4.71238898f;
			} else if (v.y==0) {
				// West
				return 0.0f;
			}
		} else if (v.x<0) {
			// QII or QIII
			if (v.y>0) {
				// QII
				return std::atan(std::fabs(float(v.x))/float(v.y)) +  1.57079633f;
			} else if (v.y<0) {
				// QIII
				return std::atan(float(v.x)/std::fabs(float(v.y))) + 4.71238898f;
			} else if (v.y==0) {
				// West, pi
				return 3.14159265f;
			}
		} else if (v.x==0) {
			// On y axis
			if (v.y>0) {
				// North, pi/2
				return 1.57079633f;
			} else if (v.y<0) {
				// South, 3*pi/2
				return 4.71238898f;
			}
		}
		// Origin
		return 0.0f;
	}
	// 3D: Return the cross product of 2 vectors
	template <class R>        vec<3,R>  cross      (const vec<3,R>& v1, const vec<3,R>& v2) {
		return vec<3,R>(
			v1.y*v2.z - v1.z*v2.y,
			v1.z*v2.x - v1.x*v2.z,
			v1.x*v2.y - v1.y*v2.x
		);
	}
	// 2D: A specialization of the 3D version which assumes
	// the input vectors have no Z component (2D) and returns
	// only the Z component of the output which has no X or Y components (scalar)
	template <class R>               R  cross      (const vec<2,R>& v1, const vec<2,R>& v2) {
		return v1.x*v2.y - v1.y*v2.x;
	}
	
	
	
	// Totally generic
	template <int N, class R>
	struct mat {
		static mat identity ();
	
		R a[N][N];
	};
	
	// 2D specialization
	template <class R>
	struct mat<2,R> {
		mat () {}
		mat (R theta);
	
		R a[2][2];
	};
	
	// Construct 2D rotation matrix
	template <class R>
	mat<2,R>::mat (R theta) {
		R cos_theta = cos(theta);
		R sin_theta = sin(theta);
		a[0][0] = cos_theta; a[0][1] = -sin_theta;
		a[1][0] = sin_theta; a[1][1] =  cos_theta;
	}

	// Matrix-vector multiplication /////////////////////////////////////////////////////////////////
	template <int N, class R>
	vec<N,R> operator *  (const mat<N,R>& lhs, const vec<N,R>& rhs) {
		vec<N,R> v;
		for (int row=0; row<N; row++) {
			v[row] = 0;
			for (int col=0; col<N; col++) {
				v[row] += lhs.a[row][col] * rhs[col];
			}
		}
		return v;
	}

	// Stream insertion /////////////////////////////////////////////////////////////////////////
	/*
		[ 1, 0 ]
		[ 0, 1 ]
	*/
	template <int N, class R>
	std::ostream& operator<< (std::ostream& os, const mat<N,R>& m) {
		os << '(' << m[0][0];
		for (int i=1; i<N; i++)
			os << ", " << m[i][i];
		os << ')';
		return os;
	}
}
