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
#include "Vector3Simple.h"
#include "Vector4Simple.h"
#include <cassert>

class Vector3Simple;
class Matrix3Simple;
//class Matrix4Intrinsics;

class Matrix4Simple	{
 public:
	inline Matrix4Simple() { ToIdentity(); }
	inline Matrix4Simple(const float (&elements)[16]) { memcpy(values, elements, 16 * sizeof(float)); }
	Matrix4Simple(const Matrix3Simple& mat);
	//Matrix4Simple(const Matrix4Intrinsics& mat);

	float	values[16];

	//Set all matrix values to zero
	inline Matrix4Simple& ToZero() { memset(values, 0, 16 * sizeof(float)); return *this; }
	//Sets matrix to identity matrix (1.0 down the diagonal)
	inline Matrix4Simple& ToIdentity() { memcpy(values, Matrix4Simple::IDENTITY_DATA, 16 * sizeof(float)); return *this;}

	inline Vector4Simple GetRow(unsigned int row) const {
		assert(("Matrix4Simple::getRow got index out of bouds (>=4)", row < 4));
		return Vector4Simple(values[row], values[row + 4], values[row + 8], values[row + 12]);
	}

	inline Vector4Simple GetCol(unsigned int column) const {
		assert(("Matrix4Simple::getColumn got index out of bouds (>=4)", column < 4));
		return Vector4Simple(values[column], values[column + 1], values[column + 2], values[column + 3]);
	}

	inline Matrix4Simple& SetRow(unsigned int row, const Vector4Simple& val) {
		assert(("Matrix4Simple::setRow got index out of bouds (>=4)", row < 4));
		values[row]		= val.x;
		values[row + 4] = val.y;
		values[row + 8] = val.z;
		values[row + 12] = val.w;
		return *this;
	}

	inline Matrix4Simple& SetCol(unsigned int column, const Vector4Simple& val) {
		assert(("Matrix4Simple::setColumn got index out of bouds (>=4)", column < 4));
		memcpy(values + (column * 4), &val, 4 * sizeof(float));
		return *this;
	}

	//Gets the OpenGL position vector (floats 12,13, and 14)
	inline Vector3Simple GetTranslation() const { return Vector3Simple(values[12], values[13], values[14]); }

	//Sets the OpenGL position vector (floats 12,13, and 14)
	inline Matrix4Simple& SetTranslation(const Vector3Simple& v) { values[12] = v.x; values[13] = v.y; values[14] = v.z; return *this; }


	//Gets the scale vector (floats 1,5, and 10)
	inline Vector3Simple GetScaling() const { return Vector3Simple(values[0], values[5], values[10]); }
	//Sets the scale vector (floats 1,5, and 10)
	inline Matrix4Simple& SetScaling(const Vector3Simple& in) { values[0] = in.x; values[5] = in.y; values[10] = in.z; return *this; }

	Matrix4Simple GetRotation() const;
	Matrix4Simple GetTransposedRotation() const;

	//Multiplies 'this' matrix by matrix 'a'. Performs the multiplication in 'OpenGL' order (ie, backwards)
	inline Matrix4Simple operator*(const Matrix4Simple& a) const {
		Matrix4Simple out;
		out.ToZero();
		//Students! You should be able to think up a really easy way of speeding this up...
		for(unsigned int r = 0; r < 4; ++r) {
			for(unsigned int c = 0; c < 4; ++c) {
				for(unsigned int i = 0; i < 4; ++i)
					out.values[c + (r * 4)] += this->values[c + (i * 4)] * a.values[(r * 4) + i];
			}
		}
		return out;
	}


	inline Vector4Simple operator*(const Vector4Simple& v) const {
		return Vector4Simple(
			v.x * values[0] + v.y * values[4] + v.z * values[8]  + v.w * values[12],
			v.x * values[1] + v.y * values[5] + v.z * values[9]  + v.w * values[13],
			v.x * values[2] + v.y * values[6] + v.z * values[10] + v.w * values[14],
			v.x * values[3] + v.y * values[7] + v.z * values[11] + v.w * values[15]
		);
	};

	inline Vector3Simple operator*(const Vector3Simple& v) const {
		Vector3Simple vec;

		vec.x = v.x*values[0] + v.y*values[4] + v.z*values[8]  + values[12];
		vec.y = v.x*values[1] + v.y*values[5] + v.z*values[9]  + values[13];
		vec.z = v.x*values[2] + v.y*values[6] + v.z*values[10] + values[14];

		float temp =  1.0f / (v.x * values[3] + v.y * values[7] + v.z * values[11] + values[15]);
		vec.x *= temp;
		vec.y *= temp;
		vec.z *= temp;

		return vec;
	};



	//Matrix4Simple operator*(const Matrix4Intrinsics& v) const;

	static const float EMPTY_DATA[16];
	static const float IDENTITY_DATA[16];
	static const Matrix4Simple EMPTY;
	static const Matrix4Simple IDENTITY;


	static Matrix4Simple RotationX(float degrees);
	static Matrix4Simple RotationY(float degrees);
	static Matrix4Simple RotationZ(float degrees);

	//Creates a rotation matrix that rotates by 'degrees' around the 'axis'
	//Analogous to glRotatef
	static Matrix4Simple Rotation(float degrees, const Vector3Simple& axis);

	//Creates a scaling matrix (puts the 'scale' vector down the diagonal)
	//Analogous to glScalef
	static Matrix4Simple Scale(const Vector3Simple& scale);

	//Creates a translation matrix (identity, with 'translation' vector at
	//floats 12, 13, and 14. Analogous to glTranslatef
	static Matrix4Simple Translation(const Vector3Simple& translation);

	//Creates a perspective matrix, with 'znear' and 'zfar' as the near and
	//far planes, using 'aspect' and 'fov' as the aspect ratio and vertical
	//field of vision, respectively.
	static Matrix4Simple Perspective(float znear, float zfar, float aspect, float fov);

	//Creates an orthographic matrix with 'znear' and 'zfar' as the near and
	//far planes, and so on. Descriptive variable names are a good thing!
	static Matrix4Simple Orthographic(float znear, float zfar,float right, float left, float top, float bottom);

	//Builds a view matrix suitable for sending straight to the vertex shader.
	//Puts the camera at 'from', with 'lookingAt' centered on the screen, with
	//'up' as the...up axis (pointing towards the top of the screen)
	static Matrix4Simple View(const Vector3Simple& from, const Vector3Simple& lookingAt, const Vector3Simple up = Vector3Simple(0,1,0));

	static Matrix4Simple Transpose(const Matrix4Simple& a);

	static Matrix4Simple Inverse(const Matrix4Simple& a);


	//Handy string output for the matrix. Can get a bit messy, but better than nothing!
	inline friend std::ostream& operator<<(std::ostream& o, const Matrix4Simple& m) {
		return o << "Mat4(" << std::endl <<
		       "\t" << m.values[0] << ", " << m.values[4] << ", " << m.values[8] << ", " << m.values [12] << ", " << std::endl <<
		       "\t" << m.values[1] << ", " << m.values[5] << ", " << m.values[9] << ", " << m.values [13]  << ", " << std::endl <<
		       "\t" << m.values[2] << ", " << m.values[6] << ", " << m.values[10] << ", " << m.values [14]  << ", " << std::endl <<
		       "\t" << m.values[3] << ", " << m.values[7] << ", " << m.values[11] << ", " << m.values [15] << std::endl <<
		       " )";
	}
};

