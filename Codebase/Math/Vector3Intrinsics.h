#pragma once

//4201-nameless structures
#pragma warning(disable: 4201)
/*
Class:Vector3
Implements:
Author:Rich Davison
Description:VERY simple Vector3 class. Students are encouraged to modify this as necessary!

-_-_-_-_-_-_-_,------,
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""

*/

#include <cmath>
#include <iostream>
#include <smmintrin.h>


/// <summary>
/// Class from http://fastcpp.blogspot.co.uk/2011/12/simple-vector3-class-with-sse-support.html
/// </summary>
class Vector3Intrinsics
{
public:
	inline Vector3Intrinsics()
	{
		ToZero();
	}
	inline Vector3Intrinsics(float x, float y, float z) : x(x), y(y), z(z) {}
	inline Vector3Intrinsics(const Vector3Intrinsics& v) : x(v.x), y(v.y), z(v.z) {}

	float x;
	float y;
	float z;

	inline Vector3Intrinsics& ToZero()
	{
		x = y = z = 0.0f;
		return *this;
	}

	inline float Length() const
	{
		__m128 vec4 = _mm_set_ps(0, z, y, x);
		return _mm_cvtss_f32(_mm_sqrt_ss(_mm_dp_ps(vec4, vec4, 0x71)));
	}
	inline float LengthR() const
	{
		__m128 vec4 = _mm_set_ps(0, z, y, x);
		return _mm_cvtss_f32(_mm_rsqrt_ss(_mm_dp_ps(vec4, vec4, 0x71)));
	}
	inline float LengthSq() const
	{
		__m128 vec4 = _mm_set_ps(0, z, y, x);
		return _mm_cvtss_f32(_mm_dp_ps(vec4, vec4, 0x71));
	}

	inline Vector3Intrinsics& Invert()
	{
		x = -x;
		y = -y;
		z = -z;
		return *this;
	}

	//TODO: can extract dot product (lengthSq) with _mm_cvtss_f32 and do a test to continue or cancel (this might save time) (might need to test for 0.00001f error margin)
	inline Vector3Intrinsics& Normalize()
	{
		__m128 temp = _mm_set_ps(0, z, y, x);
		temp = _mm_mul_ps(temp, _mm_rsqrt_ps(_mm_dp_ps(temp, temp, 0x7F)));
		x = temp.m128_f32[0];
		y = temp.m128_f32[1];
		z = temp.m128_f32[2];
		return *this;
	}
	inline Vector3Intrinsics Normal() const
	{
		__m128 temp = _mm_set_ps(0, z, y, x);
		temp = _mm_mul_ps(temp, _mm_rsqrt_ps(_mm_dp_ps(temp, temp, 0x7F)));
		return Vector3Intrinsics(temp.m128_f32[0], temp.m128_f32[1], temp.m128_f32[2]);
	}

	inline float Dot(const Vector3Intrinsics& v) const
	{
		return _mm_cvtss_f32(_mm_dp_ps(_mm_set_ps(0, z, y, x), _mm_set_ps(0, v.z, v.y, v.x), 0x71));
	}

	//http://fastcpp.blogspot.co.uk/2011/04/vector-cross-product-using-sse-code.html
	inline Vector3Intrinsics Cross(const Vector3Intrinsics& v) const
	{
		__m128 other = _mm_set_ps(0, v.z, v.y, v.x);
		__m128 temp = _mm_set_ps(0, z, y, x);
		temp = _mm_sub_ps(
		         _mm_mul_ps(_mm_shuffle_ps(temp, temp, _MM_SHUFFLE(3, 0, 2, 1)), _mm_shuffle_ps(other, other, _MM_SHUFFLE(3, 1, 0, 2))),
		         _mm_mul_ps(_mm_shuffle_ps(temp, temp, _MM_SHUFFLE(3, 1, 0, 2)), _mm_shuffle_ps(other, other, _MM_SHUFFLE(3, 0, 2, 1)))
		       );
		return Vector3Intrinsics(temp.m128_f32[0], temp.m128_f32[1], temp.m128_f32[2]);
	}


	inline Vector3Intrinsics operator+(float v) const
	{
		__m128 temp = _mm_add_ps(_mm_set_ps(0, z, y, x), _mm_set1_ps(v));
		return Vector3Intrinsics(temp.m128_f32[0], temp.m128_f32[1], temp.m128_f32[2]);
	}
	inline Vector3Intrinsics operator-(float v) const
	{
		__m128 temp = _mm_sub_ps(_mm_set_ps(0, z, y, x), _mm_set1_ps(v));
		return Vector3Intrinsics(temp.m128_f32[0], temp.m128_f32[1], temp.m128_f32[2]);
	}
	inline Vector3Intrinsics operator*(float v) const
	{
		__m128 temp = _mm_mul_ps(_mm_set_ps(0, z, y, x), _mm_set1_ps(v));
		return Vector3Intrinsics(temp.m128_f32[0], temp.m128_f32[1], temp.m128_f32[2]);
	}
	inline Vector3Intrinsics operator/(float v) const
	{
		__m128 temp = _mm_div_ps(_mm_set_ps(0, z, y, x), _mm_set1_ps(v));
		return Vector3Intrinsics(temp.m128_f32[0], temp.m128_f32[1], temp.m128_f32[2]);
	}

	inline void operator+=(float v)
	{
		__m128 temp = _mm_add_ps(_mm_set_ps(0, z, y, x), _mm_set1_ps(v));
		x = temp.m128_f32[0];
		y = temp.m128_f32[1];
		z = temp.m128_f32[2];
	}
	inline void operator-=(float v)
	{
		__m128 temp = _mm_sub_ps(_mm_set_ps(0, z, y, x), _mm_set1_ps(v));
		x = temp.m128_f32[0];
		y = temp.m128_f32[1];
		z = temp.m128_f32[2];
	}
	inline void operator*=(float v)
	{
		__m128 temp = _mm_mul_ps(_mm_set_ps(0, z, y, x), _mm_set1_ps(v));
		x = temp.m128_f32[0];
		y = temp.m128_f32[1];
		z = temp.m128_f32[2];
	}
	inline Vector3Intrinsics& operator/=(float v)
	{
		__m128 temp = _mm_div_ps(_mm_set_ps(0, z, y, x), _mm_set1_ps(v));
		x = temp.m128_f32[0];
		y = temp.m128_f32[1];
		z = temp.m128_f32[2];
	}


	inline Vector3Intrinsics operator+(const Vector3Intrinsics& v) const
	{
		__m128 temp = _mm_add_ps(_mm_set_ps(0, z, y, x), _mm_set_ps(0, v.z, v.y, v.x));
		return Vector3Intrinsics(temp.m128_f32[0], temp.m128_f32[1], temp.m128_f32[2]);
	}
	inline Vector3Intrinsics operator-(const Vector3Intrinsics& v) const
	{
		__m128 temp = _mm_sub_ps(_mm_set_ps(0, z, y, x), _mm_set_ps(0, v.z, v.y, v.x));
		return Vector3Intrinsics(temp.m128_f32[0], temp.m128_f32[1], temp.m128_f32[2]);
	}
	inline Vector3Intrinsics operator*(const Vector3Intrinsics& v) const
	{
		__m128 temp = _mm_mul_ps(_mm_set_ps(0, z, y, x), _mm_set_ps(0, v.z, v.y, v.x));
		return Vector3Intrinsics(temp.m128_f32[0], temp.m128_f32[1], temp.m128_f32[2]);
	}
	inline Vector3Intrinsics operator/(const Vector3Intrinsics& v) const
	{
		__m128 temp = _mm_div_ps(_mm_set_ps(0, z, y, x), _mm_set_ps(1.0f, v.z, v.y, v.x));
		return Vector3Intrinsics(temp.m128_f32[0], temp.m128_f32[1], temp.m128_f32[2]);
	}

	inline void operator+=(const Vector3Intrinsics& v)
	{
		__m128 temp = _mm_add_ps(_mm_set_ps(0, z, y, x), _mm_set_ps(0, v.z, v.y, v.x));
		x = temp.m128_f32[0];
		y = temp.m128_f32[1];
		z = temp.m128_f32[2];
	}
	inline Vector3Intrinsics& operator-=(const Vector3Intrinsics& v)
	{
		__m128 temp = _mm_sub_ps(_mm_set_ps(0, z, y, x), _mm_set_ps(0, v.z, v.y, v.x));
		x = temp.m128_f32[0];
		y = temp.m128_f32[1];
		z = temp.m128_f32[2];
	}
	inline Vector3Intrinsics& operator*=(const Vector3Intrinsics& v)
	{
		__m128 temp = _mm_mul_ps(_mm_set_ps(0, z, y, x), _mm_set_ps(0, v.z, v.y, v.x));
		x = temp.m128_f32[0];
		y = temp.m128_f32[1];
		z = temp.m128_f32[2];
	}
	inline Vector3Intrinsics& operator/=(const Vector3Intrinsics& v)
	{
		__m128 temp = _mm_div_ps(_mm_set_ps(0, z, y, x), _mm_set_ps(1.0f, v.z, v.y, v.x));
		x = temp.m128_f32[0];
		y = temp.m128_f32[1];
		z = temp.m128_f32[2];
	}


	/*inline Vector3Intrinsics& operator+=(float a) { x += a; y += a; z += a; return *this; }
	inline Vector3Intrinsics& operator-=(float a) { x -= a; y -= a; z -= a; return *this; }
	inline Vector3Intrinsics& operator*=(float a) { x *= a; y *= a; z *= a; return *this; }
	inline Vector3Intrinsics& operator/=(float a) { x /= a; y /= a; z /= a; return *this; }*/

	inline Vector3Intrinsics operator-() const
	{
		return Vector3Intrinsics(-x, -y, -z);
	}


	inline bool	operator==(const Vector3Intrinsics& v) const
	{
		return _mm_movemask_ps(_mm_cmpneq_ps(_mm_set_ps(0, z, y, x), _mm_set_ps(0, v.z, v.y, v.x))) == 0;
	}
	inline bool	operator!=(const Vector3Intrinsics& v) const
	{
		return _mm_movemask_ps(_mm_cmpneq_ps(_mm_set_ps(0, z, y, x), _mm_set_ps(0, v.z, v.y, v.x))) != 0;
	}


	static const Vector3Intrinsics ZEROS;
	static const Vector3Intrinsics ONES;

	inline friend std::ostream& operator<<(std::ostream& o, const Vector3Intrinsics& v)
	{
		return o << "Vector3(" << v.x << ", " << v.y << ", " << v.z << ")";
	}
};

inline Vector3Intrinsics operator+(float f, const Vector3Intrinsics& v)
{
	return v + f;
}
inline Vector3Intrinsics operator-(float f, const Vector3Intrinsics& v)
{
	return Vector3Intrinsics(f, f, f) - v;
}
inline Vector3Intrinsics operator*(float f, const Vector3Intrinsics& v)
{
	return v * f;
}
inline Vector3Intrinsics operator/(float f, const Vector3Intrinsics& v)
{
	return Vector3Intrinsics(f, f, f) / v;
}