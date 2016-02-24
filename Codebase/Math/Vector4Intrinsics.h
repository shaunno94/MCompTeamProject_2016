#ifndef ORBIS
#pragma once

//4201-nameless structure;
#pragma warning(disable: 4201)

/*
Class:Vector4
Implements:
Author:Rich Davison
Description:VERY simple Vector4 class. Students are encouraged to modify this as necessary!

-_-_-_-_-_-_-_,------,
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""

*/

#include "Memory/common.h"
#include <cmath>
#include <iostream>
#include <smmintrin.h>
#include "Vector3Intrinsics.h"

/// <summary>
/// Class from http://fastcpp.blogspot.co.uk/2011/12/simple-vector3-class-with-sse-support.html
/// </summary>
MEM_ALIGN
class Vector4Intrinsics	{
 public:
	inline Vector4Intrinsics() : mmvalue(_mm_set1_ps(1.0f)) {}

	inline Vector4Intrinsics(float x, float y, float z, float w) : mmvalue(_mm_set_ps(w, z, y, x)) {}

	inline Vector4Intrinsics(__m128 m) : mmvalue(m) {}

	inline Vector4Intrinsics(const Vector4Intrinsics& v) : mmvalue(v.mmvalue) {}

	Vector3Intrinsics ToVector3() const {
		return Vector3Intrinsics(x, y, z);
	}

	MEM_ALIGN
	struct {
		union {
			struct {
				float x;
				float y;
				float z;
				float w;
			};
			__m128 mmvalue;
		};
	};

	inline void ToZero() { mmvalue = _mm_setzero_ps(); }

	inline float Length() const { return _mm_cvtss_f32(_mm_sqrt_ss(_mm_dp_ps(mmvalue, mmvalue, 0xF1))); }
	//R for reciprocal
	inline float LengthR() const { return _mm_cvtss_f32(_mm_rsqrt_ss(_mm_dp_ps(mmvalue, mmvalue, 0xF1))); }
	inline float LengthSq() const { return _mm_cvtss_f32(_mm_dp_ps(mmvalue, mmvalue, 0xF1)); }

	//TODO: can extract dot product (lengthSq) with _mm_cvtss_f32 and do a test to continue or cancel (this might save time) (might need to test for 0.00001f error margin)
	inline void Normalize()		{ mmvalue = _mm_mul_ps(mmvalue, _mm_rsqrt_ps(_mm_dp_ps(mmvalue, mmvalue, 0xFF))); }
	inline Vector4Intrinsics Normal() const	{ return _mm_mul_ps(mmvalue, _mm_rsqrt_ps(_mm_dp_ps(mmvalue, mmvalue, 0xFF))); }

	inline float Dot(const Vector4Intrinsics& v) const { return _mm_cvtss_f32(_mm_dp_ps(mmvalue, v.mmvalue, 0xF1)); }

	inline Vector4Intrinsics operator+(float v) const { return _mm_add_ps(mmvalue, _mm_set1_ps(v)); }
	inline Vector4Intrinsics operator-(float v) const { return _mm_sub_ps(mmvalue, _mm_set1_ps(v)); }
	inline Vector4Intrinsics operator*(float v) const { return _mm_mul_ps(mmvalue, _mm_set1_ps(v)); }
	inline Vector4Intrinsics operator/(float v) const { return _mm_div_ps(mmvalue, _mm_set1_ps(v)); }

	inline void operator+=(float v) { mmvalue = _mm_add_ps(mmvalue, _mm_set1_ps(v)); }
	inline void operator-=(float v) { mmvalue = _mm_sub_ps(mmvalue, _mm_set1_ps(v)); }
	inline void operator*=(float v) { mmvalue = _mm_mul_ps(mmvalue, _mm_set1_ps(v)); }
	inline void operator/=(float v) { mmvalue = _mm_div_ps(mmvalue, _mm_set1_ps(v)); }


	inline Vector4Intrinsics operator+(const Vector4Intrinsics& v) const { return _mm_add_ps(mmvalue, v.mmvalue); }
	inline Vector4Intrinsics operator-(const Vector4Intrinsics& v) const { return _mm_sub_ps(mmvalue, v.mmvalue); }
	inline Vector4Intrinsics operator*(const Vector4Intrinsics& v) const { return _mm_mul_ps(mmvalue, v.mmvalue); }
	inline Vector4Intrinsics operator/(const Vector4Intrinsics& v) const { return _mm_div_ps(mmvalue, v.mmvalue); }

	inline void operator+=(const Vector4Intrinsics& v) { mmvalue = _mm_add_ps(mmvalue, v.mmvalue); }
	inline void operator-=(const Vector4Intrinsics& v) { mmvalue = _mm_sub_ps(mmvalue, v.mmvalue); }
	inline void operator*=(const Vector4Intrinsics& v) { mmvalue = _mm_mul_ps(mmvalue, v.mmvalue); }
	inline void operator/=(const Vector4Intrinsics& v) { mmvalue = _mm_div_ps(mmvalue, v.mmvalue); }

	inline Vector4Intrinsics operator-() const { return _mm_set_ps(-w, -z, -y, -x); }


	inline bool	operator==(const Vector4Intrinsics& v) const { return _mm_movemask_ps(_mm_cmpneq_ps(mmvalue, v.mmvalue)) == 0; }
	inline bool	operator!=(const Vector4Intrinsics& v) const { return _mm_movemask_ps(_mm_cmpneq_ps(mmvalue, v.mmvalue)) != 0; }

	MEM_ALIGN_NEW_DELETE

	static const Vector4Intrinsics ZEROS;
	static const Vector4Intrinsics ONES;

	inline friend std::ostream& operator<<(std::ostream& o, const Vector4Intrinsics& v) {
		return o << "Vector4(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w  << ")";
	}
};

inline Vector4Intrinsics operator+(float f, const Vector4Intrinsics& v) { return v + f; }
inline Vector4Intrinsics operator-(float f, const Vector4Intrinsics& v) { return Vector4Intrinsics(_mm_set1_ps(f)) - v; }
inline Vector4Intrinsics operator*(float f, const Vector4Intrinsics& v) { return v * f; }
inline Vector4Intrinsics operator/(float f, const Vector4Intrinsics& v) { return Vector4Intrinsics(_mm_set1_ps(f)) / v; }
#endif