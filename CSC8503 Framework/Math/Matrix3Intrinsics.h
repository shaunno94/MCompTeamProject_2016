#pragma once

#include "Matrix4Intrinsics.h"
#include "Vector3Intrinsics.h"
#include "Vector3_1Intrinsics.h"
#include <xmmintrin.h>
#include <cassert>

class Matrix4Intrinsics;

class Matrix3Intrinsics {
 public:
	inline Matrix3Intrinsics() { ToIdentity(); }
	inline Matrix3Intrinsics(const float (&elements)[9]) { memcpy(values, elements, 9 * sizeof(float)); }
	
	Matrix3Intrinsics(const Vector3Simple& v1, const Vector3Simple& v2, const Vector3Simple& v3);
	Matrix3Intrinsics(
		float a1, float a2, float a3,
		float b1, float b2, float b3,
		float c1, float c2, float c3);
	
	Matrix3Intrinsics(const Matrix4Intrinsics& m4);

	union
	{
		float values[9];
		struct {
			float _11, _12, _13;
			float _21, _22, _23;
			float _31, _32, _33;
		};
	};

	//Set all matrix values to zero
	inline Matrix3Intrinsics& ToZero() { memset(values, 0, 9 * sizeof(float)); return *this; }

	//Sets matrix to identity matrix (1.0 down the diagonal)
	inline Matrix3Intrinsics& ToIdentity() { memcpy(values, Matrix3Intrinsics::IDENTITY_DATA, 9 * sizeof(float)); return *this; }
	
	
	inline float   operator[](int index) const        {
		assert(("Matrix3Intrinsics::[] index out of bounds (>=9)", index < 9));
		return values[index];
	}
	inline float&  operator[](int index)              {
		assert(("Matrix3Intrinsics::[] index out of bounds (>=9)", index < 9));
		return values[index];
	}
	inline float   operator()(int row, int col) const {
		assert(("Matrix3Intrinsics::() row or column out of bounds (>=3)", row < 3 && col < 3));
		return values[row + col * 3];
	}
	inline float&  operator()(int row, int col)       {
		assert(("Matrix3Intrinsics::() row or column out of bounds (>=3)", row < 3 && col < 3));
		return values[row + col * 3];
	}

	inline Vector3Intrinsics GetRow(unsigned int row) const {
		assert(("Matrix3Intrinsics::GetRow out of bounds (row >= 3)", row < 3));
		return Vector3Intrinsics(values[row], values[row + 3], values[row + 6]);
	}

	inline Vector3Intrinsics GetCol(unsigned int column) const {
		assert(("Matrix3Intrinsics::getColumn out of bounds (column >= 3)", column < 3));
		Vector3Intrinsics out;
		memcpy(&out, &values[3 * column], sizeof(Vector3Intrinsics));
		return out;
	}

	inline Matrix3Intrinsics& SetRow(unsigned int row, const Vector3Intrinsics& val) {
		assert(("Matrix3Intrinsics::setRow out of bounds (row >= 3)", row < 3));
		values[row]		= val.x;
		values[row + 3] = val.y;
		values[row + 6] = val.z;
		return *this;
	}

	inline Matrix3Intrinsics& SetCol(unsigned int column, const Vector3Intrinsics& val) {
		assert(("Matrix3Intrinsics::setColumn out of bounds (column >= 3)", column < 3));
		memcpy(&values[column * 3], &val, sizeof(Vector3Intrinsics));
		return *this;
	}

	inline Vector3Intrinsics GetDiagonal() const { return Vector3Intrinsics(values[0], values[4], values[8]); }

	inline Matrix3Intrinsics& SetDiagonal(const Vector3Intrinsics& v) { values[0] = v.x; values[4] = v.y; values[8] = v.z; return *this; }


	inline Vector3Intrinsics operator*(const Vector3Intrinsics& v) const {
		return Vector3Intrinsics(
		         v.x * values[0] + v.y * values[3] + v.z * values[6],
		         v.x * values[1] + v.y * values[4] + v.z * values[7],
		         v.x * values[2] + v.y * values[5] + v.z * values[8]
		       );
	};

	inline Matrix3Intrinsics operator*(const Matrix3Intrinsics& m) const {
		Matrix3Intrinsics result;
		for(uint i = 0; i < 9; i += 3)
			for(uint j = 0; j < 3; ++j)
				result.values[i + j] = m.values[i] * values[j] + m.values[i + 1] * values[j + 3] + m.values[i + 2] * values[j + 6];

		return result;
	};

	inline Matrix3Intrinsics& Transpose() {
		__m128 empty = _mm_setzero_ps();
		__m128 column1 = _mm_loadu_ps(&values[0]);
		__m128 column2 = _mm_loadu_ps(&values[3]);
		__m128Converter column3;
		column3.mmvalue = _mm_loadu_ps(&values[6]);

		_MM_TRANSPOSE4_PS(column1, column2, column3.mmvalue, empty);

		_mm_storeu_ps(&values[0], column1);
		_mm_storeu_ps(&values[3], column2);
		values[6] = column3.x;
		values[7] = column3.y;
		values[8] = column3.z;

		return *this;
	}

	float Determinant() const;

	Matrix3Intrinsics Inverse() const;

	bool TryInvert();

	bool TryTransposedInvert();

	static const float EMPTY_DATA[9];
	static const float IDENTITY_DATA[9];

	static const Matrix3Intrinsics EMPTY;
	static const Matrix3Intrinsics IDENTITY;

	static Matrix3Intrinsics RotationX(float degrees);
	static Matrix3Intrinsics RotationY(float degrees);
	static Matrix3Intrinsics RotationZ(float degrees);

	//Creates a rotation matrix that rotates by 'degrees' around the 'axis'
	//Analogous to glRotatef
	static Matrix3Intrinsics Rotation(float degrees, const Vector3Intrinsics& axis);

	static Matrix3Intrinsics Rotation(float degreesX, float degreesY, float degreesZ);

	//Creates a scaling matrix (puts the 'scale' vector down the diagonal)
	//Analogous to glScalef
	static Matrix3Intrinsics Scale(const Vector3Intrinsics& scale);

	//Handy string output for the matrix. Can get a bit messy, but better than nothing!
	friend std::ostream& operator<<(std::ostream& o, const Matrix3Intrinsics& m);
};

