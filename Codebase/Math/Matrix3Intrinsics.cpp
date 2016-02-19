#ifndef ORBIS
#include "Matrix3Intrinsics.h"

#include "Matrix4Intrinsics.h"
#include "Helpers/degrees.h"
#include "Helpers/common.h"
#include <cstring>

const float Matrix3Intrinsics::EMPTY_DATA[9] = {
	0, 0, 0,
	0, 0, 0,
	0, 0, 0
};

const float Matrix3Intrinsics::IDENTITY_DATA[9] = {
	1, 0, 0,
	0, 1, 0,
	0, 0, 1
};

const Matrix3Intrinsics Matrix3Intrinsics::EMPTY = Matrix3Intrinsics(Matrix3Intrinsics::EMPTY_DATA);
const Matrix3Intrinsics Matrix3Intrinsics::IDENTITY = Matrix3Intrinsics(Matrix3Intrinsics::IDENTITY_DATA);



Matrix3Intrinsics::Matrix3Intrinsics(
  const Vector3Simple& v1,
  const Vector3Simple& v2,
  const Vector3Simple& v3) {
	const unsigned int size = 3 * sizeof(float);
	memcpy(&values[0], &v1.x, size);
	memcpy(&values[3], &v2.x, size);
	memcpy(&values[6], &v3.x, size);
}

Matrix3Intrinsics::Matrix3Intrinsics(
  float a1, float a2, float a3,
  float b1, float b2, float b3,
  float c1, float c2, float c3) :
	_11(a1), _12(a2), _13(a3),
	_21(b1), _22(b2), _23(b3),
	_31(c1), _32(c2), _33(c3) {}


Matrix3Intrinsics::Matrix3Intrinsics(const Matrix4Intrinsics& m4) {
	_mm_storeu_ps(&values[0], m4.mmvalues[0]);
	_mm_storeu_ps(&values[3], m4.mmvalues[1]);
	values[6] = m4.values[8];
	values[7] = m4.values[9];
	values[8] = m4.values[10];
}


Matrix3Intrinsics Matrix3Intrinsics::Rotation(float degrees, const Vector3Intrinsics& axis) {
	Matrix3Intrinsics m;
	Vector3Intrinsics axisNorm = axis.Normal();
	float c = cos(DegToRad(degrees));
	float s = sin(DegToRad(degrees));

	__m128 normXYZW = _mm_set_ps(0, axisNorm.z, axisNorm.y, axisNorm.x);
	__m128 normXYZWWithC = _mm_mul_ps(normXYZW, _mm_set1_ps(1.0f - c));
	__m128 col0 = _mm_mul_ps(normXYZW, _mm_set1_ps(normXYZWWithC.m128_f32[0]));
	__m128 col1 = _mm_mul_ps(normXYZW, _mm_set1_ps(normXYZWWithC.m128_f32[1]));
	__m128 col2 = _mm_mul_ps(normXYZW, _mm_set1_ps(normXYZWWithC.m128_f32[2]));

	m.values[0] = col0.m128_f32[0] + c;
	m.values[1] = col0.m128_f32[1] + (axisNorm.z * s);
	m.values[2] = col0.m128_f32[2] - (axisNorm.y * s);
	m.values[3] = col1.m128_f32[0] - (axisNorm.z * s);
	m.values[4] = col1.m128_f32[1] + c;
	m.values[5] = col1.m128_f32[2] + (axisNorm.x * s);
	m.values[6] = col2.m128_f32[0] + (axisNorm.y * s);
	m.values[7] = col2.m128_f32[1] - (axisNorm.x * s);
	m.values[8] = col2.m128_f32[2] + c;
	return m;
}

Matrix3Intrinsics Matrix3Intrinsics::Rotation(float degreesX, float degreesY, float degreesZ) {
	// Building this matrix directly is faster than multiplying three matrices for X, Y and Z
	float phi = DegToRad(degreesX), theta = DegToRad(degreesY), psi = DegToRad(degreesZ);
	float sinTh = sin(theta), cosTh = cos(theta),
	      sinPh = sin(phi), cosPh = cos(phi),
	      sinPs = sin(psi), cosPs = cos(psi);

	Matrix3Intrinsics result;
	result.values[0] = cosTh * cosPs;
	result.values[1] = cosTh * sinPs;
	result.values[2] = -sinTh;
	result.values[3] = -cosPh * sinPs + sinPh * sinTh * cosPs;
	result.values[4] = cosPh * cosPs + sinPh * sinTh * sinPs;
	result.values[5] = sinPh * cosTh;
	result.values[6] = sinPh * sinPs + cosPh * sinTh * cosPs;
	result.values[7] = -sinPh * cosPs + cosPh * sinTh * sinPs;
	result.values[8] = cosPh * cosTh;
	return result;
}

Matrix3Intrinsics Matrix3Intrinsics::RotationX(float degrees)	 {
	Matrix3Intrinsics m;
	float rad = DegToRad(degrees);
	float c = cos(rad);
	float s = sin(rad);

	m.values[4] = c;
	m.values[5] = s;

	m.values[7] = -s;
	m.values[8] = c;

	return m;
}

Matrix3Intrinsics Matrix3Intrinsics::RotationY(float degrees)	 {
	Matrix4Intrinsics m;
	float rad = DegToRad(degrees);
	float c = cos(rad);
	float s = sin(rad);

	m.values[0] = c;
	m.values[2] = s;

	m.values[6] = -s;
	m.values[8] = c;

	return m;
}

Matrix3Intrinsics Matrix3Intrinsics::RotationZ(float degrees)	 {
	Matrix4Intrinsics m;
	float rad = DegToRad(degrees);
	float c = cos(rad);
	float s = sin(rad);

	m.values[0] = c;
	m.values[1] = -s;

	m.values[3] = s;
	m.values[4] = c;

	return m;
}

Matrix3Intrinsics Matrix3Intrinsics::Scale(const Vector3Intrinsics& scale) {
	Matrix3Intrinsics m;
	m.SetDiagonal(scale);
	return m;
}


float Matrix3Intrinsics::Determinant() const {
	return
	  values[0] * (values[4] * values[8] - values[5] * values[7]) -
	  values[3] * (values[1] * values[8] - values[7] * values[2]) +
	  values[6] * (values[1] * values[5] - values[4] * values[2]);
}

Matrix3Intrinsics Matrix3Intrinsics::Inverse() const {
	Matrix3Intrinsics result;
	float det = Determinant();
	if(det != 0.0f) {
		float detReciprocal = 1.0f / det;
		result.values[0] = (values[4] * values[8] - values[5] * values[7]) * detReciprocal;
		result.values[1] = (values[7] * values[2] - values[1] * values[8]) * detReciprocal;
		result.values[2] = (values[1] * values[5] - values[2] * values[4]) * detReciprocal;
		result.values[3] = (values[6] * values[5] - values[3] * values[8]) * detReciprocal;
		result.values[4] = (values[0] * values[8] - values[6] * values[2]) * detReciprocal;
		result.values[5] = (values[2] * values[3] - values[0] * values[5]) * detReciprocal;
		result.values[6] = (values[3] * values[7] - values[6] * values[4]) * detReciprocal;
		result.values[7] = (values[1] * values[6] - values[0] * values[7]) * detReciprocal;
		result.values[8] = (values[0] * values[4] - values[1] * values[3]) * detReciprocal;
	}
	return result;
}

bool Matrix3Intrinsics::TryInvert() {
	float det = Determinant();
	if(det != 0.0f) {
		float detReciprocal = 1.0f / det;
		float result[9];
		result[0] = (values[4] * values[8] - values[5] * values[7]) * detReciprocal;
		result[1] = (values[7] * values[2] - values[1] * values[8]) * detReciprocal;
		result[2] = (values[1] * values[5] - values[2] * values[4]) * detReciprocal;
		result[3] = (values[6] * values[5] - values[3] * values[8]) * detReciprocal;
		result[4] = (values[0] * values[8] - values[6] * values[2]) * detReciprocal;
		result[5] = (values[2] * values[3] - values[0] * values[5]) * detReciprocal;
		result[6] = (values[3] * values[7] - values[6] * values[4]) * detReciprocal;
		result[7] = (values[1] * values[6] - values[0] * values[7]) * detReciprocal;
		result[8] = (values[0] * values[4] - values[1] * values[3]) * detReciprocal;
		memcpy(values, result, 9 * sizeof(float));
		return true;
	}
	return false;
}

bool Matrix3Intrinsics::TryTransposedInvert() {
	float det = Determinant();
	if(det != 0.0f) {
		float invdet = 1.0f / det;
		float result[9];
		result[0] = (values[4] * values[8] - values[5] * values[7]) * invdet;
		result[1] = (values[6] * values[5] - values[3] * values[8]) * invdet;
		result[2] = (values[3] * values[7] - values[6] * values[4]) * invdet;
		result[3] = (values[7] * values[2] - values[1] * values[8]) * invdet;
		result[4] = (values[0] * values[8] - values[6] * values[2]) * invdet;
		result[5] = (values[1] * values[6] - values[0] * values[7]) * invdet;
		result[6] = (values[1] * values[5] - values[2] * values[4]) * invdet;
		result[7] = (values[2] * values[3] - values[0] * values[5]) * invdet;
		result[8] = (values[0] * values[4] - values[1] * values[3]) * invdet;
		memcpy(values, result, 9 * sizeof(float));
		return true;
	}
	return false;
}


std::ostream& operator<<(std::ostream& o, const Matrix3Intrinsics& m) {
	return o << "Mat3(" << LINE_SEPARATOR <<
	       "\t" << m.values[0] << ", " << m.values[3] << ", " << m.values[6] << ", " << LINE_SEPARATOR <<
	       "\t" << m.values[1] << ", " << m.values[4] << ", " << m.values[7] << ", " << LINE_SEPARATOR <<
	       "\t" << m.values[2] << ", " << m.values[5] << ", " << m.values[8] << LINE_SEPARATOR <<
	       ")";
}
#endif