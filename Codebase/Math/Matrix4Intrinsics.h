/******************************************************************************
Class:Matrix4
Implements:
Author:Rich Davison
Description:VERY simple 4 by 4 matrix class. Students are encouraged to modify
this as necessary! Overloading the [] operator to allow acces to the values
array in a neater way might be a good start, as the floats that make the matrix
up are currently public.

-_-_-_-_-_-_-_,------,
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""

*//////////////////////////////////////////////////////////////////////////////
#pragma once

#include <iostream>
#include "Vector3Intrinsics.h"
#include "Vector3Simple.h"
#include "Vector3_1Intrinsics.h"
#include "Vector4Intrinsics.h"
#include "Vector4Simple.h"
#include <xmmintrin.h>
#include <cassert>

class Vector3Intrinsics;
class Matrix3Intrinsics;
class Matrix4Simple;

MEM_ALIGN
class Matrix4Intrinsics
{
public:
	inline Matrix4Intrinsics()
	{
		ToIdentity();
	}
	inline Matrix4Intrinsics(const float (&elements)[16])
	{
		memcpy(values, elements, 16 * sizeof(float));
	}

	Matrix4Intrinsics(const Matrix3Intrinsics& mat);
	Matrix4Intrinsics(const Matrix4Simple& mat);

	MEM_ALIGN
	struct {
		union {
			float values[16];
			__m128 mmvalues[4];
		};
	};

	//Set all matrix values to zero
	inline void ToZero()
	{
		mmvalues[0] = mmvalues[1] = mmvalues[2] = mmvalues[3] = _mm_setzero_ps();
	}
	//Sets matrix to identity matrix (1.0 down the diagonal)
	inline void ToIdentity()
	{
		//This is slightly faster than memset (27% faster in debug; 12% faster in release;)
		mmvalues[0] = _mm_load_ps(Matrix4Intrinsics::IDENTITY_DATA);
		mmvalues[1] = _mm_load_ps(Matrix4Intrinsics::IDENTITY_DATA + 4);
		mmvalues[2] = _mm_load_ps(Matrix4Intrinsics::IDENTITY_DATA + 8);
		mmvalues[3] = _mm_load_ps(Matrix4Intrinsics::IDENTITY_DATA + 12);
	}

	inline Vector4Intrinsics GetRow(unsigned int row) const
	{
		assert(("Matrix4Intrinsics::getRow got index out of bouds", row < 4));
		return Vector4Intrinsics(values[row], values[row + 4], values[row + 8], values[row + 12]);
	}

	inline Vector4Intrinsics GetCol(unsigned int column) const
	{
		assert(("Matrix4Intrinsics::getColumn got index out of bouds", column < 4));
		return Vector4Intrinsics(mmvalues[column]);
	}

	inline void SetRow(unsigned int row, const Vector4Intrinsics& val)
	{
		assert(("Matrix4Intrinsics::setRow got index out of bouds", row < 4));
		values[row]		= val.x;
		values[row + 4] = val.y;
		values[row + 8] = val.z;
		values[row + 12] = val.w;
	}

	inline void SetCol(unsigned int column, const Vector4Intrinsics& val)
	{
		assert(("Matrix4Intrinsics::setColumn got index out of bouds", column < 4));
		mmvalues[column] = val.mmvalue;
	}

	//Gets the OpenGL position vector (floats 12,13, and 14)
	//inline Vector3Intrinsics GetTranslation() const { return Vector3Intrinsics(values[12], values[13], values[14]); }
	inline Vector3Simple GetTranslation() const
	{
		return Vector3Simple(values[12], values[13], values[14]);
	}

	inline Vector3_1Intrinsics GetTranslationMemAligned() const
	{
		return Vector3_1Intrinsics(mmvalues[3]);
	}

	//Sets the OpenGL position vector (floats 12,13, and 14)
	inline void SetTranslation(const Vector3Intrinsics& v)
	{
		values[12] = v.x;
		values[13] = v.y;
		values[14] = v.z;
	}

	//Sets the OpenGL position vector (floats 12,13, and 14)
	inline void SetTranslation(const Vector3_1Intrinsics& v)
	{
		values[12] = v.x;
		values[13] = v.y;
		values[14] = v.z;
	}
	//Sets the OpenGL position vector (floats 12,13, and 14)
	inline void SetTranslation(const Vector3Simple& v)
	{
		values[12] = v.x;
		values[13] = v.y;
		values[14] = v.z;
	}

	//Gets the scale vector (floats 1,5, and 10)
	inline Vector3Intrinsics GetScaling() const
	{
		return Vector3Intrinsics(values[0], values[5], values[10]);
	}
	//Gets the scale vector (floats 1,5, and 10)
	inline Vector3_1Intrinsics GetScalingMemAligned() const
	{
		return Vector3_1Intrinsics(values[0], values[5], values[10]);
	}

	//Sets the scale vector (floats 1,5, and 10)
	inline void SetScaling(const Vector3Intrinsics& in)
	{
		values[0] = in.x;
		values[5] = in.y;
		values[10] = in.z;
	}
	//Sets the scale vector (floats 1,5, and 10)
	inline void SetScaling(const Vector3_1Intrinsics& in)
	{
		values[0] = in.x;
		values[5] = in.y;
		values[10] = in.z;
	}

	Matrix4Intrinsics GetRotation() const;

	//Multiplies 'this' matrix by matrix 'a'. Performs the multiplication in 'OpenGL' order (ie, backwards)
	Matrix4Intrinsics operator*(const Matrix4Intrinsics& m) const;

	Vector3Intrinsics operator*(const Vector3Intrinsics& v) const;
	
	Vector3Simple operator*(const Vector3Simple& v) const;

	Vector3_1Intrinsics operator*(const Vector3_1Intrinsics& v) const;

	Vector4Intrinsics operator*(const Vector4Intrinsics& v) const;

	Vector4Simple operator*(const Vector4Simple& v) const;

	void Transpose();

	static Vector3Intrinsics GetEulerAngles(const Matrix4Intrinsics& mat);

	static const MEM_ALIGN float Matrix4Intrinsics::EMPTY_DATA[16];
	static const MEM_ALIGN float Matrix4Intrinsics::IDENTITY_DATA[16];
	static const Matrix4Intrinsics EMPTY;
	static const Matrix4Intrinsics IDENTITY;

	static Matrix4Intrinsics RotationX(float degrees);
	static Matrix4Intrinsics RotationY(float degrees);
	static Matrix4Intrinsics RotationZ(float degrees);


	//Creates a rotation matrix that rotates by 'degrees' around the 'axis'
	//Analogous to glRotatef
	static Matrix4Intrinsics Rotation(float degrees, const Vector3Intrinsics& axis);
	static Matrix4Intrinsics Rotation(float degrees, const Vector3Simple& axis);

	static Matrix4Intrinsics Rotation(float degreesX, float degreesY, float degreesZ);

	//Creates a scaling matrix (puts the 'scale' vector down the diagonal)
	//Analogous to glScalef
	static Matrix4Intrinsics Scale(const Vector3Intrinsics& scale);
	static Matrix4Intrinsics Scale(const Vector3Simple& scale);

	//Creates a translation matrix (identity, with 'translation' vector at
	//floats 12, 13, and 14. Analogous to glTranslatef
	static Matrix4Intrinsics Translation(const Vector3Intrinsics& translation);
	static Matrix4Intrinsics Translation(const Vector3Simple& translation);

	//Creates a perspective matrix, with 'znear' and 'zfar' as the near and
	//far planes, using 'aspect' and 'fov' as the aspect ratio and vertical
	//field of vision, respectively.
	static Matrix4Intrinsics Perspective(float znear, float zfar, float aspect, float fov);

	//Creates an orthographic matrix with 'znear' and 'zfar' as the near and
	//far planes, and so on. Descriptive variable names are a good thing!
	static Matrix4Intrinsics Orthographic(float znear, float zfar, float right, float left, float top, float bottom);

	//Builds a view matrix suitable for sending straight to the vertex shader.
	//Puts the camera at 'from', with 'lookingAt' centered on the screen, with
	//'up' as the...up axis (pointing towards the top of the screen)
	static Matrix4Intrinsics View(const Vector3Intrinsics& from, const Vector3Intrinsics& lookingAt, const Vector3Intrinsics& up = Vector3Intrinsics(0.0f, 1.0f, 0.0f));


	MEM_ALIGN_NEW_DELETE

	//Handy string output for the matrix. Can get a bit messy, but better than nothing!
	friend std::ostream& operator<<(std::ostream& o, const Matrix4Intrinsics& m);
};
