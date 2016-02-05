/******************************************************************************
Class:Quaternion
Implements:
Author:Rich Davison
Description:VERY simple Quaternion class. Students are encouraged to modify
this as necessary...if they dare.

Quaternions aren't really discussed much in the graphics module, but the
MD5Mesh class uses them internally, as MD5 files store their rotations
as quaternions.

I hate Quaternions.

-_-_-_-_-_-_-_,------,
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""

*//////////////////////////////////////////////////////////////////////////////

#pragma once

#include <iostream>
#include "Helpers/common.h"
#include "Matrix4Intrinsics.h"
#include "Matrix3Intrinsics.h"
#include <smmintrin.h>

class Matrix4Intrinsics;

MEM_ALIGN
class QuaternionIntrinsics	{
 public:
	inline QuaternionIntrinsics() : mmvalue(_mm_set_ps(1.0f, 0, 0, 0)) {}
	inline QuaternionIntrinsics(float x, float y, float z, float w) : mmvalue(_mm_set_ps(w, z, y, x)) {}
	inline QuaternionIntrinsics(__m128 m) : mmvalue(m) {}
	inline QuaternionIntrinsics(const QuaternionIntrinsics& v) : mmvalue(v.mmvalue) {}

	MEM_ALIGN
	union {
		struct {
			float x, y, z, w;
		};
		__m128 mmvalue;
	};

	inline void Normalize()		{ mmvalue = _mm_mul_ps(mmvalue, _mm_rsqrt_ps(_mm_dp_ps(mmvalue, mmvalue, 0xFF))); }
	inline QuaternionIntrinsics Normal() const	{ return _mm_mul_ps(mmvalue, _mm_rsqrt_ps(_mm_dp_ps(mmvalue, mmvalue, 0xFF))); }

	Matrix4Intrinsics ToMatrix4() const;
	Matrix3Intrinsics ToMatrix3() const;


	QuaternionIntrinsics	Conjugate() const { return QuaternionIntrinsics(-x,-y,-z,w); }

	void		GenerateW();	//builds 4th component when loading in shortened, 3 component quaternions

	static const QuaternionIntrinsics EMPTY;
	static const QuaternionIntrinsics IDENTITY;

	static QuaternionIntrinsics EulerAnglesToQuaternion(float pitch, float yaw, float roll);
	static QuaternionIntrinsics AxisAngleToQuaterion(const Vector3Intrinsics& vector, float degrees);

	static void RotatePointByQuaternion(const QuaternionIntrinsics& q, Vector3Intrinsics& point);

	static QuaternionIntrinsics FromMatrix(const Matrix4Intrinsics& m);

	inline float Dot(const QuaternionIntrinsics& q) const { return _mm_cvtss_f32(_mm_dp_ps(mmvalue, q.mmvalue, 0xF1));}

	//http://stackoverflow.com/questions/18542894/how-to-multiply-two-quaternions-with-minimal-instructions
	inline QuaternionIntrinsics operator*(const QuaternionIntrinsics& q) const {
		__m128 wzyx = _mm_shuffle_ps(mmvalue, mmvalue, _MM_SHUFFLE(0,1,2,3));
		__m128 baba = _mm_shuffle_ps(q.mmvalue, q.mmvalue, _MM_SHUFFLE(0,1,0,1));
		__m128 dcdc = _mm_shuffle_ps(q.mmvalue, q.mmvalue, _MM_SHUFFLE(2,3,2,3));
		__m128 ZnXWY = _mm_hsub_ps(_mm_mul_ps(mmvalue, baba), _mm_mul_ps(wzyx, dcdc));

		__m128 XZYnW = _mm_hadd_ps(_mm_mul_ps(mmvalue, dcdc), _mm_mul_ps(wzyx, baba));
		__m128 XZWY = _mm_addsub_ps(_mm_shuffle_ps(XZYnW, ZnXWY, _MM_SHUFFLE(3,2,1,0)), _mm_shuffle_ps(ZnXWY, XZYnW, _MM_SHUFFLE(2,3,0,1)));

		return _mm_shuffle_ps(XZWY, XZWY, _MM_SHUFFLE(2,1,3,0));
	}

	inline QuaternionIntrinsics operator*(const Vector3Intrinsics& v) const {
		/*return QuaternionIntrinsics(
		         (w * q.x) + (y * q.z) - (z * q.y),
		         (w * q.y) + (z * q.x) - (x * q.z),
		         (w * q.z) + (x * q.y) - (y * q.x),
		         -(x * q.x) - (y * q.y) - (z * q.z)
		       );
		*/
		//This (^) is equiv to q * v, where the below is equiv to v * q (needed for physics)
		return QuaternionIntrinsics(
		         (w * v.x) + (v.y * z) - (v.z * y),
		         (w * v.y) + (v.z * x) - (v.x * z),
		         (w * v.z) + (v.x * y) - (v.y * x),
		         -(x * v.x) - (y * v.y) - (z * v.z)
		       );
	}

	inline QuaternionIntrinsics operator+(const QuaternionIntrinsics& a) const {
		return _mm_add_ps(mmvalue, a.mmvalue);
	}

	QuaternionIntrinsics Interpolate(const QuaternionIntrinsics& pStart, const QuaternionIntrinsics& pEnd, float pFactor);


	MEM_ALIGN_NEW_DELETE

	inline friend std::ostream& operator<<(std::ostream& o, const QuaternionIntrinsics& q) {
		return o << "Quat(" << q.x << "," << q.y << "," << q.z <<  "," << q.w << ")" << LINE_SEPARATOR;
	}
};