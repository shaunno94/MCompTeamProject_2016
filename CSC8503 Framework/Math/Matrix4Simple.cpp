#include "Matrix4Simple.h"

#include "Matrix3Simple.h"
#include "Matrix4Intrinsics.h"

const float Matrix4Simple::EMPTY_DATA[16] = {
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0
};

const float Matrix4Simple::IDENTITY_DATA[16] = {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
};

const Matrix4Simple Matrix4Simple::EMPTY = Matrix4Simple(Matrix4Simple::EMPTY_DATA);
const Matrix4Simple Matrix4Simple::IDENTITY = Matrix4Simple(Matrix4Simple::IDENTITY_DATA);

Matrix4Simple::Matrix4Simple(const Matrix3Simple& mat) {
	const unsigned int size = 3 * sizeof(float);
	memcpy(&values[0], &mat.values[0], size);
	memcpy(&values[4], &mat.values[3], size);
	memcpy(&values[8], &mat.values[6], size);
	values[3] = values[7] = values[12] = values[13] = values[14] = 0.0f;
	values[15] = 1.0f;
}

Matrix4Simple::Matrix4Simple(const Matrix4Intrinsics& mat) {
	memcpy(&values[0], &mat.values[0], sizeof(Matrix4Intrinsics));
}

Matrix4Simple Matrix4Simple::GetRotation() const {
	Matrix4Simple result;
	result.values[0] = values[0];
	result.values[5] = values[5];
	result.values[10] = values[10];
	result.values[1] = values[1];
	result.values[2] = values[2];
	result.values[4] = values[4];
	result.values[8] = values[8];
	result.values[6] = values[6];
	result.values[9] = values[9];
	return result;
}
Matrix4Simple Matrix4Simple::GetTransposedRotation() const {
	Matrix4Simple result;
	result.values[0] = values[0];
	result.values[5] = values[5];
	result.values[10] = values[10];
	result.values[1] = values[4];
	result.values[2] = values[8];
	result.values[4] = values[1];
	result.values[8] = values[2];
	result.values[6] = values[9];
	result.values[9] = values[6];
	return result;
}


Matrix4Simple Matrix4Simple::operator*(const Matrix4Intrinsics& v) const {
	return *this * Matrix4Simple(v.values);
};


Matrix4Simple Matrix4Simple::Perspective(float znear, float zfar, float aspect, float fov) {
	Matrix4Simple m;

	const float h = 1.0f / tan(fov * PI_OVER_360);
	float neg_depth = znear - zfar;

	m.values[0]		= h / aspect;
	m.values[5]		= h;
	m.values[10]	= (zfar + znear) / neg_depth;
	m.values[11]	= -1.0f;
	m.values[14]	= 2.0f * (znear * zfar) / neg_depth;
	m.values[15]	= 0.0f;

	return m;
}

//http://www.opengl.org/sdk/docs/man/xhtml/glOrtho.xml
Matrix4Simple Matrix4Simple::Orthographic(float znear, float zfar, float right, float left, float top, float bottom)	{
	Matrix4Simple m;

	m.values[0]	= 2.0f / (right - left);
	m.values[5]	= 2.0f / (top - bottom);
	m.values[10]	= -2.0f / (zfar - znear);

	m.values[12]  = -(right + left) / (right - left);
	m.values[13]  = -(top + bottom) / (top - bottom);
	m.values[14]  = -(zfar + znear) / (zfar - znear);
	m.values[15]  = 1.0f;

	return m;
}

Matrix4Simple Matrix4Simple::View(const Vector3Simple& from, const Vector3Simple& lookingAt, const Vector3Simple up /*= Vector3(1,0,0)*/)	{
	Matrix4Simple r;
	r.SetTranslation(Vector3Simple(-from.x, -from.y, -from.z));

	Matrix4Simple m;

	Vector3Simple f = (lookingAt - from);
	f.Normalize();

	Vector3Simple s = f.Cross(up);
	Vector3Simple u = s.Cross(f);

	s.Normalize();
	u.Normalize();

	m.values[0] = s.x;
	m.values[4] = s.y;
	m.values[8] = s.z;

	m.values[1] = u.x;
	m.values[5] = u.y;
	m.values[9] = u.z;

	m.values[2]  = -f.x;
	m.values[6]  = -f.y;
	m.values[10] = -f.z;

	return m * r;
}

Matrix4Simple Matrix4Simple::RotationX(float degrees)	 {
	Matrix4Simple m;
	float rad = DegToRad(degrees);
	float c = cos(rad);
	float s = sin(rad);

	m.values[5] = c;
	m.values[6] = s;

	m.values[9] = -s;
	m.values[10] = c;

	return m;
}

Matrix4Simple Matrix4Simple::RotationY(float degrees)	 {
	Matrix4Simple m;
	float rad = DegToRad(degrees);
	float c = cos(rad);
	float s = sin(rad);

	m.values[0] = c;
	m.values[2] = s;

	m.values[8] = -s;
	m.values[10] = c;

	return m;
}

Matrix4Simple Matrix4Simple::RotationZ(float degrees)	 {
	Matrix4Simple m;
	float rad = DegToRad(degrees);
	float c = cos(rad);
	float s = sin(rad);

	m.values[0] = c;
	m.values[1] = -s;

	m.values[4] = s;
	m.values[5] = c;

	return m;
}

Matrix4Simple Matrix4Simple::Rotation(float degrees, const Vector3Simple& inaxis)	 {
	Matrix4Simple m;

	Vector3Simple axis = inaxis;

	axis.Normalize();

	float c = cos((float)DegToRad(degrees));
	float s = sin((float)DegToRad(degrees));

	m.values[0]  = (axis.x * axis.x) * (1.0f - c) + c;
	m.values[1]  = (axis.y * axis.x) * (1.0f - c) + (axis.z * s);
	m.values[2]  = (axis.z * axis.x) * (1.0f - c) - (axis.y * s);

	m.values[4]  = (axis.x * axis.y) * (1.0f - c) - (axis.z * s);
	m.values[5]  = (axis.y * axis.y) * (1.0f - c) + c;
	m.values[6]  = (axis.z * axis.y) * (1.0f - c) + (axis.x * s);

	m.values[8]  = (axis.x * axis.z) * (1.0f - c) + (axis.y * s);
	m.values[9]  = (axis.y * axis.z) * (1.0f - c) - (axis.x * s);
	m.values[10] = (axis.z * axis.z) * (1.0f - c) + c;

	return m;
}

Matrix4Simple Matrix4Simple::Scale(const Vector3Simple& scale)	{
	Matrix4Simple m;

	m.values[0]  = scale.x;
	m.values[5]  = scale.y;
	m.values[10] = scale.z;

	return m;
}

Matrix4Simple Matrix4Simple::Translation(const Vector3Simple& translation)	{
	Matrix4Simple m;

	m.values[12] = translation.x;
	m.values[13] = translation.y;
	m.values[14] = translation.z;

	return m;
}

Matrix4Simple& Matrix4Simple::Transpose() {
	Matrix4Simple temp;
	temp.values[0] = values[0];
	temp.values[5] = values[5];
	temp.values[10] = values[10];
	temp.values[15] = values[15];

	temp.values[1] = values[4];
	temp.values[4] = values[1];
	temp.values[2] = values[8];
	temp.values[8] = values[2];
	temp.values[3] = values[12];
	temp.values[12] = values[3];
	temp.values[6] = values[9];
	temp.values[9] = values[6];
	temp.values[7] = values[13];
	temp.values[13] = values[7];
	temp.values[11] = values[14];
	temp.values[14] = values[11];

	*this = temp;
	return *this;
}