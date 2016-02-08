#include "Matrix3Simple.h"

#include "Matrix4Simple.h"
#include "Helpers/degrees.h"
#include "Helpers/common.h"

const float Matrix3Simple::EMPTY_DATA[9] = {
	0, 0, 0,
	0, 0, 0,
	0, 0, 0
};
const float Matrix3Simple::IDENTITY_DATA[9] = {
	1, 0, 0,
	0, 1, 0,
	0, 0, 1
};

const Matrix3Simple Matrix3Simple::EMPTY = Matrix3Simple(Matrix3Simple::EMPTY_DATA);
const Matrix3Simple Matrix3Simple::IDENTITY = Matrix3Simple(Matrix3Simple::IDENTITY_DATA);

Matrix3Simple::Matrix3Simple(
  const Vector3Simple& v1,
  const Vector3Simple& v2,
  const Vector3Simple& v3) {
	const unsigned int size = 3 * sizeof(float);
	memcpy(&values[0], &v1.x, size);
	memcpy(&values[3], &v2.x, size);
	memcpy(&values[6], &v3.x, size);
}

Matrix3Simple::Matrix3Simple(
  float a1, float a2, float a3,
  float b1, float b2, float b3,
  float c1, float c2, float c3) :
	_11(a1), _12(a2), _13(a3),
	_21(b1), _22(b2), _23(b3),
	_31(c1), _32(c2), _33(c3) {}

Matrix3Simple::Matrix3Simple(const Matrix4Simple& m4) {
	values[0] = m4.values[0];
	values[1] = m4.values[1];
	values[2] = m4.values[2];

	values[3] = m4.values[4];
	values[4] = m4.values[5];
	values[5] = m4.values[6];

	values[6] = m4.values[8];
	values[7] = m4.values[9];
	values[8] = m4.values[10];
}


Matrix3Simple Matrix3Simple::Rotation(float degrees, const Vector3Simple& inaxis)	 {
	Matrix3Simple m;

	Vector3Simple axis = inaxis;

	axis.Normalize();

	float c = cos((float)DegToRad(degrees));
	float s = sin((float)DegToRad(degrees));

	m.values[0]  = (axis.x * axis.x) * (1.0f - c) + c;
	m.values[1]  = (axis.y * axis.x) * (1.0f - c) + (axis.z * s);
	m.values[2]  = (axis.z * axis.x) * (1.0f - c) - (axis.y * s);

	m.values[3]  = (axis.x * axis.y) * (1.0f - c) - (axis.z * s);
	m.values[4]  = (axis.y * axis.y) * (1.0f - c) + c;
	m.values[5]  = (axis.z * axis.y) * (1.0f - c) + (axis.x * s);

	m.values[6]  = (axis.x * axis.z) * (1.0f - c) + (axis.y * s);
	m.values[7]  = (axis.y * axis.z) * (1.0f - c) - (axis.x * s);
	m.values[8]  = (axis.z * axis.z) * (1.0f - c) + c;

	return m;
}

Matrix3Simple Matrix3Simple::Scale(const Vector3Simple& scale)	{
	Matrix3Simple m;
	m.SetDiagonal(scale);
	return m;
}

float Matrix3Simple::Determinant() const {
	return
	  values[0] * (values[4] * values[8] - values[5] * values[7]) -
	  values[3] * (values[1] * values[8] - values[7] * values[2]) +
	  values[6] * (values[1] * values[5] - values[4] * values[2]);
}

float Matrix3Simple::Trace() const
{
	return _11 + _22 + _33;
}

Matrix3Simple Matrix3Simple::Inverse(const Matrix3Simple& mat) {
	Matrix3Simple result;
	float det = mat.Determinant();
	if(det != 0.0f) {
		float detReciprocal = 1.0f / det;
		result.values[0] = (mat.values[4] * mat.values[8] - mat.values[5] * mat.values[7]) * detReciprocal;
		result.values[1] = (mat.values[7] * mat.values[2] - mat.values[1] * mat.values[8]) * detReciprocal;
		result.values[2] = (mat.values[1] * mat.values[5] - mat.values[2] * mat.values[4]) * detReciprocal;
		result.values[3] = (mat.values[6] * mat.values[5] - mat.values[3] * mat.values[8]) * detReciprocal;
		result.values[4] = (mat.values[0] * mat.values[8] - mat.values[6] * mat.values[2]) * detReciprocal;
		result.values[5] = (mat.values[2] * mat.values[3] - mat.values[0] * mat.values[5]) * detReciprocal;
		result.values[6] = (mat.values[3] * mat.values[7] - mat.values[6] * mat.values[4]) * detReciprocal;
		result.values[7] = (mat.values[1] * mat.values[6] - mat.values[0] * mat.values[7]) * detReciprocal;
		result.values[8] = (mat.values[0] * mat.values[4] - mat.values[1] * mat.values[3]) * detReciprocal;
	}
	return result;
}

bool Matrix3Simple::TryInvert() {
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

bool Matrix3Simple::TryTransposedInvert() {
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

Matrix3Simple& Matrix3Simple::Transpose() {
	float temp3 = values[3];
	float temp6 = values[6];
	float temp1 = values[1];
	float temp7 = values[7];
	float temp2 = values[2];
	float temp5 = values[5];
	values[1] = temp3;
	values[2] = temp6;
	values[3] = temp1;
	values[4] = temp7;
	values[6] = temp2;
	values[7] = temp5;
	return *this;
}


Matrix3Simple Matrix3Simple::Transpose(const Matrix3Simple& mat) {
	Matrix3Simple result;

	result.values[0] = mat.values[0];
	result.values[4] = mat.values[4];
	result.values[8] = mat.values[8];

	result.values[1] = mat.values[3];
	result.values[2] = mat.values[6];
	result.values[3] = mat.values[1];
	result.values[5] = mat.values[7];
	result.values[6] = mat.values[2];
	result.values[7] = mat.values[5];

	return result;
}

Matrix3Simple Matrix3Simple::RotationX(float degrees)	 {
	Matrix3Simple m;
	float rad = DegToRad(degrees);
	float c = cos(rad);
	float s = sin(rad);

	m.values[4] = c;
	m.values[5] = s;

	m.values[7] = -s;
	m.values[8] = c;

	return m;
}

Matrix3Simple Matrix3Simple::RotationY(float degrees)	 {
	Matrix3Simple m;
	float rad = DegToRad(degrees);
	float c = cos(rad);
	float s = sin(rad);

	m.values[0] = c;
	m.values[2] = s;

	m.values[6] = -s;
	m.values[8] = c;

	return m;
}

Matrix3Simple Matrix3Simple::RotationZ(float degrees)	 {
	Matrix3Simple m;
	float rad = DegToRad(degrees);
	float c = cos(rad);
	float s = sin(rad);

	m.values[0] = c;
	m.values[1] = -s;

	m.values[3] = s;
	m.values[4] = c;

	return m;
}

static Matrix3Simple Adjugate(const Matrix3Simple& m) {
	Matrix3Simple adj;

	adj._11 = m._22 * m._33 - m._23 * m._32;
	adj._12 = m._13 * m._32 - m._12 * m._33;
	adj._13 = m._12 * m._23 - m._13 * m._22;

	adj._21 = m._23 * m._31 - m._21 * m._33;
	adj._22 = m._11 * m._33 - m._13 * m._31;
	adj._23 = m._13 * m._21 - m._11 * m._23;

	adj._31 = m._21 * m._32 - m._22 * m._31;
	adj._32 = m._12 * m._31 - m._11 * m._32;
	adj._33 = m._11 * m._22 - m._12 * m._21;

	return adj;
}

Matrix3Simple Matrix3Simple::OuterProduct(const Vector3Simple& a, const Vector3Simple& b)
{
	Matrix3Simple m;

	m._11 = a.x * b.x;
	m._12 = a.x * b.y;
	m._13 = a.x * b.z;

	m._21 = a.y * b.x;
	m._22 = a.y * b.y;
	m._23 = a.y * b.z;

	m._31 = a.z * b.x;
	m._32 = a.z * b.y;
	m._33 = a.z * b.z;

	return m;
}

std::ostream& operator<<(std::ostream& o, const Matrix3Simple& m) {
	return o << "Mat3(" << LINE_SEPARATOR <<
	       "\t" << m.values[0] << ", " << m.values[3] << ", " << m.values[6] << ", " << LINE_SEPARATOR <<
	       "\t" << m.values[1] << ", " << m.values[4] << ", " << m.values[7] << ", " << LINE_SEPARATOR <<
	       "\t" << m.values[2] << ", " << m.values[5] << ", " << m.values[8] << LINE_SEPARATOR <<
	       ")";
}


Matrix3Simple& operator+=(Matrix3Simple& a, const Matrix3Simple& b) {
	for (unsigned int i = 0; i < 9; ++i)
		a.values[i] += b.values[i];
	return a;
}
Matrix3Simple& operator-=(Matrix3Simple& a, const Matrix3Simple& b){
	for (unsigned int i = 0; i < 9; ++i)
		a.values[i] -= b.values[i];
	return a;
}

Matrix3Simple operator+(const Matrix3Simple& a, const Matrix3Simple& b){
	Matrix3Simple m;
	for (unsigned int i = 0; i < 9; ++i)
		m.values[i] = a.values[i] + b.values[i];
	return m;
}
Matrix3Simple operator-(const Matrix3Simple& a, const Matrix3Simple& b){
	Matrix3Simple m;
	for (unsigned int i = 0; i < 9; ++i)
		m.values[i] = a.values[i] - b.values[i];
	return m;
}


Matrix3Simple& operator+=(Matrix3Simple& a, const float b){
	for (unsigned int i = 0; i < 9; ++i)
		a.values[i] += b;
	return a;
}
Matrix3Simple& operator-=(Matrix3Simple& a, const float b){
	for (unsigned int i = 0; i < 9; ++i)
		a.values[i] -= b;
	return a;
}
Matrix3Simple& operator*=(Matrix3Simple& a, const float b){
	for (unsigned int i = 0; i < 9; ++i)
		a.values[i] *= b;
	return a;
}
Matrix3Simple& operator/=(Matrix3Simple& a, const float b){
	float bReciprocal = 1.0f / b;
	for (unsigned int i = 0; i < 9; ++i)
		a.values[i] *= bReciprocal;
	return a;
}

Matrix3Simple operator+(const Matrix3Simple& a, const float b){
	Matrix3Simple m;
	for (unsigned int i = 0; i < 9; ++i)
		m.values[i] = a.values[i] + b;
	return m;
}
Matrix3Simple operator-(const Matrix3Simple& a, const float b){
	Matrix3Simple m;
	for (unsigned int i = 0; i < 9; ++i)
		m.values[i] = a.values[i] - b;
	return m;
}
Matrix3Simple operator*(const Matrix3Simple& a, const float b){
	Matrix3Simple m;
	for (unsigned int i = 0; i < 9; ++i)
		m.values[i] = a.values[i] * b;
	return m;
}
Matrix3Simple operator/(const Matrix3Simple& a, const float b){
	Matrix3Simple m;
	for (unsigned int i = 0; i < 9; ++i)
		m.values[i] = a.values[i] / b;
	return m;
}