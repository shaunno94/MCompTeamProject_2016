#pragma once
#include <cassert>
#include "Vector3Simple.h"

class Matrix4Simple;

class Matrix3Simple {
 public:
	inline Matrix3Simple() { ToIdentity(); }
	inline Matrix3Simple(const float (&elements)[9]) { memcpy(values, elements, 9 * sizeof(float)); }

	Matrix3Simple(const Vector3Simple& v1, const Vector3Simple& v2, const Vector3Simple& v3);
	Matrix3Simple(
	  float a1, float a2, float a3,
	  float b1, float b2, float b3,
	  float c1, float c2, float c3);
	Matrix3Simple(const Matrix4Simple& m4);

	union {
		float values[9];
		struct {
			float _11, _12, _13;
			float _21, _22, _23;
			float _31, _32, _33;
		};
	};


	//Set all matrix values to zero
	inline void	ToZero() { memset(values, 0, 9 * sizeof(float)); }
	//Sets matrix to identity matrix (1.0 down the diagonal)
	inline void	ToIdentity() { memcpy(values, Matrix3Simple::IDENTITY_DATA, 9 * sizeof(float)); }


	inline float   operator[](int index) const        {
		assert(("Matrix3Simple::[] index out of bounds (>=9)", index < 9));
		return values[index];
	}
	inline float&  operator[](int index)              {
		assert(("Matrix3Simple::[] index out of bounds (>=9)", index < 9));
		return values[index];
	}
	inline float   operator()(int row, int col) const {
		assert(("Matrix3Simple::() row or column out of bounds (>=3)", row < 3 && col < 3));
		return values[row + col * 3];
	}
	inline float&  operator()(int row, int col)       {
		assert(("Matrix3Simple::() row or column out of bounds (>=3)", row < 3 && col < 3));
		return values[row + col * 3];
	}


	inline Vector3Simple GetRow(unsigned int row) const {
		assert(("Matrix3Simple::GetRow index out of bounds (>=3)", row < 3));
		return Vector3Simple(values[row], values[row + 3], values[row + 6]);
	}
	inline Matrix3Simple& SetRow(unsigned int row, const Vector3Simple& val) {
		assert(("Matrix3Simple::SetRow index out of bounds (>=3)", row < 3));
		values[row]		= val.x;
		values[row + 3] = val.y;
		values[row + 6] = val.z;
		return *this;
	}
	inline Vector3Simple GetCol(unsigned int column) const {
		assert(("Matrix3Simple::GetColumn index out of bounds (>=3)", column < 3));
		Vector3Simple out;
		memcpy(&out, &values[3 * column], sizeof(Vector3Simple));
		return out;
	}
	inline Matrix3Simple& SetCol(unsigned int column, const Vector3Simple& val) {
		assert(("Matrix3Simple::SetColumn out of bounds (column >= 3)", column < 3));
		memcpy(&values[column * 3], &val, sizeof(Vector3Simple));
		return *this;
	}


	inline Vector3Simple GetDiagonal() const { return Vector3Simple(values[0], values[4], values[8]); }

	inline Matrix3Simple& SetDiagonal(const Vector3Simple& in) { values[0] = in.x; values[4] = in.y; values[8] = in.z; return *this; }


	inline Vector3Simple operator*(const Vector3Simple& v) const {
		return Vector3Simple(
		         v.x * values[0] + v.y * values[3] + v.z * values[6],
		         v.x * values[1] + v.y * values[4] + v.z * values[7],
		         v.x * values[2] + v.y * values[5] + v.z * values[8]
		       );
	};
	inline Matrix3Simple operator*(const Matrix3Simple& m) const {
		Matrix3Simple result;
		for(unsigned int i = 0; i < 9; i += 3)
			for(unsigned int j = 0; j < 3; ++j)
				result.values[i + j] = m.values[i] * values[j] + m.values[i + 1] * values[j + 3] + m.values[i + 2] * values[j + 6];

		return result;
	};

	float Determinant() const;
	float Trace() const;

	bool TryInvert();
	bool TryTransposedInvert();

	Matrix3Simple& Transpose();

	static const float EMPTY_DATA[9];
	static const float IDENTITY_DATA[9];

	static const Matrix3Simple EMPTY;
	static const Matrix3Simple IDENTITY;

	static Matrix3Simple RotationX(float degrees);
	static Matrix3Simple RotationY(float degrees);
	static Matrix3Simple RotationZ(float degrees);

	//Creates a rotation matrix that rotates by 'degrees' around the 'axis'
	//Analogous to glRotatef
	static Matrix3Simple Rotation(float degrees, const Vector3Simple& axis);

	//Creates a scaling matrix (puts the 'scale' vector down the diagonal)
	//Analogous to glScalef
	static Matrix3Simple Scale(const Vector3Simple& scale);

	
	static Matrix3Simple Inverse(const Matrix3Simple& rhs);
	static Matrix3Simple Transpose(const Matrix3Simple& rhs);
	static Matrix3Simple Adjugate(const Matrix3Simple& m);

	static Matrix3Simple OuterProduct(const Vector3Simple& a, const Vector3Simple& b);

	void DebugDrawMatrix(const Vector3Simple& position);

	//Handy string output for the matrix. Can get a bit messy, but better than nothing!
	friend std::ostream& operator<<(std::ostream& o, const Matrix3Simple& m);
};

Matrix3Simple& operator+=(Matrix3Simple& a, const Matrix3Simple& b);
Matrix3Simple& operator-=(Matrix3Simple& a, const Matrix3Simple& b);

Matrix3Simple operator+(const Matrix3Simple& a, const Matrix3Simple& b);
Matrix3Simple operator-(const Matrix3Simple& a, const Matrix3Simple& b);


Matrix3Simple& operator+=(Matrix3Simple& a, const float b);
Matrix3Simple& operator-=(Matrix3Simple& a, const float b);
Matrix3Simple& operator*=(Matrix3Simple& a, const float b);
Matrix3Simple& operator/=(Matrix3Simple& a, const float b);

Matrix3Simple operator+(const Matrix3Simple& a, const float b);
Matrix3Simple operator-(const Matrix3Simple& a, const float b);
Matrix3Simple operator*(const Matrix3Simple& a, const float b);
Matrix3Simple operator/(const Matrix3Simple& a, const float b);
