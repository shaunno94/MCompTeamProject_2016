#pragma once

//4201-nameless structures; 4324-structure being padded for alignment
#pragma warning(disable: 4201 4324)

#include <cmath>
#include <iostream>
#include <smmintrin.h>
#include "Vector3Intrinsics.h"

class Vector3Simple;

/// <summary>
/// Class from http://fastcpp.blogspot.co.uk/2011/12/simple-Vector3_1-class-with-sse-support.html
/// </summary>
MEM_ALIGN
class Vector3_1Intrinsics	{
 public:
	inline Vector3_1Intrinsics() : mmvalue(_mm_setzero_ps()) {}
	inline Vector3_1Intrinsics(float x, float y, float z) : mmvalue(_mm_set_ps(0, z, y, x)) {}
	inline Vector3_1Intrinsics(const Vector3Intrinsics& v) : mmvalue(_mm_set_ps(0, v.z, v.y, v.x)) {}
	inline Vector3_1Intrinsics(__m128 m) : mmvalue(m) {}
	inline Vector3_1Intrinsics(const Vector3_1Intrinsics& v) : mmvalue(v.mmvalue) {}

	Vector3_1Intrinsics(const Vector3Simple& v);

	MEM_ALIGN
	struct {
		union {
			struct {
				float x;
				float y;
				float z;
			};
			__m128 mmvalue;
		};
	};


	inline void ToZero() { mmvalue = _mm_setzero_ps(); }

	inline float Length() const { return _mm_cvtss_f32(_mm_sqrt_ss(_mm_dp_ps(mmvalue, mmvalue, 0x71))); }
	inline float LengthR() const { return _mm_cvtss_f32(_mm_rsqrt_ss(_mm_dp_ps(mmvalue, mmvalue, 0x71))); }
	inline float LengthSq() const { return _mm_cvtss_f32(_mm_dp_ps(mmvalue, mmvalue, 0x71)); }

	inline void Invert() {mmvalue = _mm_mul_ps(mmvalue, _mm_set1_ps(-1.0f)); }

	//TODO: can extract dot product (lengthSq) with _mm_cvtss_f32 and do a test to continue or cancel (this might save time) (might need to test for 0.00001f error margin)
	inline void Normalize() { mmvalue = _mm_mul_ps(mmvalue, _mm_rsqrt_ps(_mm_dp_ps(mmvalue, mmvalue, 0x77))); }
	inline Vector3_1Intrinsics Normal() const	{ return _mm_mul_ps(mmvalue, _mm_rsqrt_ps(_mm_dp_ps(mmvalue, mmvalue, 0x77))); }

	inline float Dot(const Vector3_1Intrinsics& v) const { return _mm_cvtss_f32(_mm_dp_ps(mmvalue, v.mmvalue, 0x71)); }

	//http://fastcpp.blogspot.co.uk/2011/04/vector-cross-product-using-sse-code.html
	inline Vector3_1Intrinsics Cross(const Vector3_1Intrinsics& v) const {
		return _mm_sub_ps(
		         _mm_mul_ps(_mm_shuffle_ps(mmvalue, mmvalue, _MM_SHUFFLE(3, 0, 2, 1)), _mm_shuffle_ps(v.mmvalue, v.mmvalue, _MM_SHUFFLE(3, 1, 0, 2))),
		         _mm_mul_ps(_mm_shuffle_ps(mmvalue, mmvalue, _MM_SHUFFLE(3, 1, 0, 2)), _mm_shuffle_ps(v.mmvalue, v.mmvalue, _MM_SHUFFLE(3, 0, 2, 1)))
		       );
	}


	inline Vector3_1Intrinsics operator+(float v) const { return _mm_add_ps(mmvalue, _mm_set1_ps(v)); }
	inline Vector3_1Intrinsics operator-(float v) const { return _mm_sub_ps(mmvalue, _mm_set1_ps(v)); }
	inline Vector3_1Intrinsics operator*(float v) const { return _mm_mul_ps(mmvalue, _mm_set1_ps(v)); }
	inline Vector3_1Intrinsics operator/(float v) const { return _mm_div_ps(mmvalue, _mm_set1_ps(v)); }

	inline void  operator+=(float v) { mmvalue = _mm_add_ps(mmvalue, _mm_set1_ps(v)); }
	inline void operator-=(float v) { mmvalue = _mm_sub_ps(mmvalue, _mm_set1_ps(v)); }
	inline void operator*=(float v) { mmvalue = _mm_mul_ps(mmvalue, _mm_set1_ps(v)); }
	inline void operator/=(float v) { mmvalue = _mm_div_ps(mmvalue, _mm_set1_ps(v)); }


	inline Vector3_1Intrinsics operator+(const Vector3_1Intrinsics& v) const { return _mm_add_ps(mmvalue, v.mmvalue); }
	inline Vector3_1Intrinsics operator-(const Vector3_1Intrinsics& v) const { return _mm_sub_ps(mmvalue, v.mmvalue); }
	inline Vector3_1Intrinsics operator*(const Vector3_1Intrinsics& v) const { return _mm_mul_ps(mmvalue, v.mmvalue); }
	inline Vector3_1Intrinsics operator/(const Vector3_1Intrinsics& v) const { return _mm_div_ps(mmvalue, v.mmvalue); }

	inline void operator+=(const Vector3_1Intrinsics& v) { mmvalue = _mm_add_ps(mmvalue, v.mmvalue); }
	inline void operator-=(const Vector3_1Intrinsics& v) { mmvalue = _mm_sub_ps(mmvalue, v.mmvalue); }
	inline void operator*=(const Vector3_1Intrinsics& v) { mmvalue = _mm_mul_ps(mmvalue, v.mmvalue); }
	inline void operator/=(const Vector3_1Intrinsics& v) { mmvalue = _mm_div_ps(mmvalue, v.mmvalue); }

	inline Vector3_1Intrinsics operator-() const { return _mm_set_ps(0, -z, -y, -x); }


	inline bool	operator==(const Vector3_1Intrinsics& v) const { return (_mm_movemask_ps(_mm_cmpneq_ps(mmvalue, v.mmvalue)) & 0x7) == 0; }
	inline bool	operator!=(const Vector3_1Intrinsics& v) const { return (_mm_movemask_ps(_mm_cmpneq_ps(mmvalue, v.mmvalue)) & 0x7) != 0; }

	MEM_ALIGN_NEW_DELETE


	static const Vector3_1Intrinsics ZEROS;
	static const Vector3_1Intrinsics ONES;

	inline friend std::ostream& operator<<(std::ostream& o, const Vector3_1Intrinsics& v) {
		return o << "Vector3_1(" << v.x << ", " << v.y << ", " << v.z << ")";
	}
};

inline Vector3_1Intrinsics operator+(float f, const Vector3_1Intrinsics& v) { return v + f; }
inline Vector3_1Intrinsics operator-(float f, const Vector3_1Intrinsics& v) { return Vector3_1Intrinsics(_mm_set1_ps(f)) - v; }
inline Vector3_1Intrinsics operator*(float f, const Vector3_1Intrinsics& v) { return v * f; }
inline Vector3_1Intrinsics operator/(float f, const Vector3_1Intrinsics& v) { return Vector3_1Intrinsics(_mm_set1_ps(f)) / v; }