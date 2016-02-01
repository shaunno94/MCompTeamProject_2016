#include "Matrix4Intrinsics.h"

#include "Matrix3Intrinsics.h"
#include "Matrix4Simple.h"

const float Matrix4Intrinsics::EMPTY_DATA[16] = {
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0
};

const float Matrix4Intrinsics::IDENTITY_DATA[16] = {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
};

const Matrix4Intrinsics Matrix4Intrinsics::EMPTY = Matrix4Intrinsics(Matrix4Intrinsics::EMPTY_DATA);
const Matrix4Intrinsics Matrix4Intrinsics::IDENTITY = Matrix4Intrinsics(Matrix4Intrinsics::IDENTITY_DATA);

Matrix4Intrinsics::Matrix4Intrinsics(const Matrix3Intrinsics& mat)
{
	const unsigned int size = 3 * sizeof(float);
	memcpy(&values[0], &mat.values[0], size);
	memcpy(&values[4], &mat.values[3], size);
	memcpy(&values[8], &mat.values[6], size);
	values[3] = values[7] = values[12] = values[13] = values[14] = 0.0f;
	values[15] = 1.0f;
}


Matrix4Intrinsics::Matrix4Intrinsics(const Matrix4Simple& mat)
{
	memcpy(&values[0], &mat.values[0], sizeof(Matrix4Simple));
}

Matrix4Intrinsics Matrix4Intrinsics::operator*(const Matrix4Intrinsics& m) const
{
	//http://fhtr.blogspot.co.uk/2010/02/4x4-float-matrix-multiplication-using.html
	Matrix4Intrinsics result = EMPTY;
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			result.mmvalues[j] = _mm_add_ps(_mm_mul_ps(_mm_set1_ps(m.values[j * 4 + i]), mmvalues[i]), result.mmvalues[j]);
	return result;
}

Vector3Intrinsics Matrix4Intrinsics::operator*(const Vector3Intrinsics& v) const
{
	__m128 m0 = _mm_mul_ps(mmvalues[0], _mm_set1_ps(v.x));
	__m128 m1 = _mm_mul_ps(mmvalues[1], _mm_set1_ps(v.y));
	__m128 m2 = _mm_mul_ps(mmvalues[2], _mm_set1_ps(v.z));

	__m128 tempValue = _mm_add_ps(_mm_add_ps(m0, m1), _mm_add_ps(m2, mmvalues[3]));
	tempValue = _mm_div_ps(tempValue, _mm_shuffle_ps(tempValue, tempValue, _MM_SHUFFLE(0, 0, 0, 0)));
	return Vector3Intrinsics(tempValue.m128_f32[0], tempValue.m128_f32[1], tempValue.m128_f32[2]);
};

Vector3Simple Matrix4Intrinsics::operator*(const Vector3Simple& v) const
{
	__m128 m0 = _mm_mul_ps(mmvalues[0], _mm_set1_ps(v.x));
	__m128 m1 = _mm_mul_ps(mmvalues[1], _mm_set1_ps(v.y));
	__m128 m2 = _mm_mul_ps(mmvalues[2], _mm_set1_ps(v.z));

	__m128 tempValue = _mm_add_ps(_mm_add_ps(m0, m1), _mm_add_ps(m2, mmvalues[3]));
	tempValue = _mm_div_ps(tempValue, _mm_shuffle_ps(tempValue, tempValue, _MM_SHUFFLE(0, 0, 0, 0)));
	return Vector3Simple(tempValue.m128_f32[0], tempValue.m128_f32[1], tempValue.m128_f32[2]);
};

Vector3_1Intrinsics Matrix4Intrinsics::operator*(const Vector3_1Intrinsics& v) const
{
	__m128 m0 = _mm_mul_ps(mmvalues[0], _mm_set1_ps(v.x));
	__m128 m1 = _mm_mul_ps(mmvalues[1], _mm_set1_ps(v.y));
	__m128 m2 = _mm_mul_ps(mmvalues[2], _mm_set1_ps(v.z));

	__m128 tempValue = _mm_add_ps(_mm_add_ps(m0, m1), _mm_add_ps(m2, mmvalues[3]));
	return _mm_div_ps(tempValue, _mm_shuffle_ps(tempValue, tempValue, _MM_SHUFFLE(0, 0, 0, 0)));
};

Vector4Intrinsics Matrix4Intrinsics::operator*(const Vector4Intrinsics& v) const
{
	__m128 m0 = _mm_mul_ps(mmvalues[0], _mm_set1_ps(v.x));
	__m128 m1 = _mm_mul_ps(mmvalues[1], _mm_set1_ps(v.y));
	__m128 m2 = _mm_mul_ps(mmvalues[2], _mm_set1_ps(v.z));
	__m128 m3 = _mm_mul_ps(mmvalues[3], _mm_set1_ps(v.w));

	return Vector4Intrinsics(_mm_add_ps(_mm_add_ps(m0, m1), _mm_add_ps(m2, m3)));
};

Vector4Simple Matrix4Intrinsics::operator*(const Vector4Simple& v) const
{
	__m128 m0 = _mm_mul_ps(mmvalues[0], _mm_set1_ps(v.x));
	__m128 m1 = _mm_mul_ps(mmvalues[1], _mm_set1_ps(v.y));
	__m128 m2 = _mm_mul_ps(mmvalues[2], _mm_set1_ps(v.z));
	__m128 m3 = _mm_mul_ps(mmvalues[3], _mm_set1_ps(v.w));

	__m128 tempValue = _mm_add_ps(_mm_add_ps(m0, m1), _mm_add_ps(m2, m3));
	return Vector4Simple(tempValue.m128_f32[0], tempValue.m128_f32[1], tempValue.m128_f32[2], tempValue.m128_f32[3]);
};

void Matrix4Intrinsics::Transpose()
{
	_MM_TRANSPOSE4_PS(mmvalues[0], mmvalues[1], mmvalues[2], mmvalues[3]);
}

Matrix4Intrinsics Matrix4Intrinsics::GetRotation() const
{
	Matrix4Intrinsics result(*this);
	result.SetTranslation(Vector3Intrinsics::ZEROS);
	result.values[3] = result.values[7] = result.values[11] = 0.0f;
	result.values[15] = 1.0f;
	return result;
}

//http://www.flipcode.com/documents/matrfaq.html#Q37
Vector3Intrinsics Matrix4Intrinsics::GetEulerAngles(const Matrix4Intrinsics& mat)
{
	float angle_x = 0.0f;
	float angle_y = -asin( mat.values[8]);        /* Calculate Y-axis angle */
	float angle_z = 0.0f;

	float c = cos( angle_y );
	angle_y *= RAD;

	if (fabs(c) > 0.005) { /* Gimball lock? */
		c = 1.0f / c;
		float tr_x = mat.values[10] * c;           /* No, so get X-axis angle */
		float tr_y = -mat.values[9] * c;

		angle_x = atan2( tr_y, tr_x ) * RAD;

		tr_x = mat.values[0] * c;            /* Get Z-axis angle */
		tr_y = -mat.values[4] * c;

		angle_z = atan2(tr_y, tr_x) * RAD;
	} else {								/* Gimball lock has occurred */
		float tr_x = mat.values[5];    /* And calculate Z-axis angle */
		float tr_y = mat.values[1];

		angle_z = atan2(tr_y, tr_x) * RAD;
	}

	return Vector3Intrinsics(Clamp(angle_x, 0.0f, 360.0f), Clamp(angle_y, 0.0f, 360.0f), Clamp(angle_z, 0.0f, 360.0f));
}

Matrix4Intrinsics Matrix4Intrinsics::Perspective(float znear, float zfar, float aspect, float fov)
{
	Matrix4Intrinsics m;
	const float h = 1.0f / tan(fov * PI_OVER_360);
	float neg_depth_r = 1.0f / (znear - zfar);

	m.values[0]	= h / aspect;
	m.values[5]	= h;
	m.values[10] = (zfar + znear) * neg_depth_r;
	m.values[11] = -1.0f;
	m.values[14] = 2.0f * (znear * zfar) * neg_depth_r;
	m.values[15] = 0.0f;

	return m;
}

//http://www.opengl.org/sdk/docs/man/xhtml/glOrtho.xml
Matrix4Intrinsics Matrix4Intrinsics::Orthographic(float znear, float zfar, float right, float left, float top, float bottom)
{
	Matrix4Intrinsics m;
	float x_r = 1.0f / (right - left);
	float y_r = 1.0f / (top - bottom);
	float z_r = 1.0f / (zfar - znear);

	m.values[0] = 2.0f * x_r;
	m.values[5]	= 2.0f * y_r;
	m.values[10] = -2.0f * z_r;

	m.values[12] = -(right + left) * x_r;
	m.values[13] = -(top + bottom) * y_r;
	m.values[14] = -(zfar + znear) * z_r;
	m.values[15] = 1.0f;

	return m;
}

Matrix4Intrinsics Matrix4Intrinsics::View(const Vector3Intrinsics& from, const Vector3Intrinsics& lookingAt, const Vector3Intrinsics& up)
{
	Matrix4Intrinsics r;
	r.SetTranslation(Vector3Intrinsics(-from.x, -from.y, -from.z));

	Vector3Intrinsics f = (lookingAt - from);
	f.Normalize();

	Vector3Intrinsics s = f.Cross(up);
	s.Normalize();
	Vector3Intrinsics u = s.Cross(f);
	u.Normalize();

	Matrix4Intrinsics m;

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

Matrix4Intrinsics Matrix4Intrinsics::RotationX(float degrees)
{
	Matrix4Intrinsics m;
	float rad = DegToRad(degrees);
	float c = cos(rad);
	float s = sin(rad);

	m.values[5] = c;
	m.values[6] = s;

	m.values[9] = -s;
	m.values[10] = c;

	return m;
}

Matrix4Intrinsics Matrix4Intrinsics::RotationY(float degrees)
{
	Matrix4Intrinsics m;
	float rad = DegToRad(degrees);
	float c = cos(rad);
	float s = sin(rad);

	m.values[0] = c;
	m.values[2] = s;

	m.values[8] = -s;
	m.values[10] = c;

	return m;
}

Matrix4Intrinsics Matrix4Intrinsics::RotationZ(float degrees)
{
	Matrix4Intrinsics m;
	float rad = DegToRad(degrees);
	float c = cos(rad);
	float s = sin(rad);

	m.values[0] = c;
	m.values[1] = -s;

	m.values[4] = s;
	m.values[5] = c;

	return m;
}

Matrix4Intrinsics Matrix4Intrinsics::Rotation(float degrees, const Vector3Intrinsics& axis)
{
	Vector3Intrinsics axisNorm = axis.Normal();
	float rad = DegToRad(degrees);
	float c = cos(rad);
	float s = sin(rad);

	__m128 normXYZW = _mm_set_ps(0, axisNorm.z, axisNorm.y, axisNorm.x);
	__m128 normXYZWWithC = _mm_mul_ps(normXYZW, _mm_set1_ps(1.0f - c));
	__m128 col0 = _mm_mul_ps(normXYZW, _mm_set1_ps(normXYZWWithC.m128_f32[0]));
	__m128 col1 = _mm_mul_ps(normXYZW, _mm_set1_ps(normXYZWWithC.m128_f32[1]));
	__m128 col2 = _mm_mul_ps(normXYZW, _mm_set1_ps(normXYZWWithC.m128_f32[2]));

	Matrix4Intrinsics m;
	m.values[0] = col0.m128_f32[0] + c;
	m.values[1] = col0.m128_f32[1] + (axisNorm.z * s);
	m.values[2] = col0.m128_f32[2] - (axisNorm.y * s);
	m.values[4] = col1.m128_f32[0] - (axisNorm.z * s);
	m.values[5] = col1.m128_f32[1] + c;
	m.values[6] = col1.m128_f32[2] + (axisNorm.x * s);
	m.values[8] = col2.m128_f32[0] + (axisNorm.y * s);
	m.values[9] = col2.m128_f32[1] - (axisNorm.x * s);
	m.values[10] = col2.m128_f32[2] + c;

	return m;
}
Matrix4Intrinsics Matrix4Intrinsics::Rotation(float degrees, const Vector3Simple& axis)
{
	Vector3Simple axisNorm = axis.Normal();
	float rad = DegToRad(degrees);
	float c = cos(rad);
	float s = sin(rad);

	__m128 normXYZW = _mm_set_ps(0, axisNorm.z, axisNorm.y, axisNorm.x);
	__m128 normXYZWWithC = _mm_mul_ps(normXYZW, _mm_set1_ps(1.0f - c));
	__m128 col0 = _mm_mul_ps(normXYZW, _mm_set1_ps(normXYZWWithC.m128_f32[0]));
	__m128 col1 = _mm_mul_ps(normXYZW, _mm_set1_ps(normXYZWWithC.m128_f32[1]));
	__m128 col2 = _mm_mul_ps(normXYZW, _mm_set1_ps(normXYZWWithC.m128_f32[2]));

	Matrix4Intrinsics m;
	m.values[0] = col0.m128_f32[0] + c;
	m.values[1] = col0.m128_f32[1] + (axisNorm.z * s);
	m.values[2] = col0.m128_f32[2] - (axisNorm.y * s);
	m.values[4] = col1.m128_f32[0] - (axisNorm.z * s);
	m.values[5] = col1.m128_f32[1] + c;
	m.values[6] = col1.m128_f32[2] + (axisNorm.x * s);
	m.values[8] = col2.m128_f32[0] + (axisNorm.y * s);
	m.values[9] = col2.m128_f32[1] - (axisNorm.x * s);
	m.values[10] = col2.m128_f32[2] + c;

	return m;
}

Matrix4Intrinsics Matrix4Intrinsics::Rotation(float degreesX, float degreesY, float degreesZ)
{
	// Building this matrix directly is faster than multiplying three matrices for X, Y and Z
	float phi = DegToRad(degreesX), theta = DegToRad(degreesY), psi = DegToRad(degreesZ);
	float sinTh = sin(theta), cosTh = cos(theta),
	      sinPh = sin(phi), cosPh = cos(phi),
	      sinPs = sin(psi), cosPs = cos(psi);

	Matrix4Intrinsics result;
	result.values[0] = cosTh * cosPs;
	result.values[1] = cosTh * sinPs;
	result.values[2] = -sinTh;
	result.values[4] = -cosPh * sinPs + sinPh * sinTh * cosPs;
	result.values[5] = cosPh * cosPs + sinPh * sinTh * sinPs;
	result.values[6] = sinPh * cosTh;
	result.values[8] = sinPh * sinPs + cosPh * sinTh * cosPs;
	result.values[9] = -sinPh * cosPs + cosPh * sinTh * sinPs;
	result.values[10] = cosPh * cosTh;
	return result;
}

Matrix4Intrinsics Matrix4Intrinsics::Scale(const Vector3Intrinsics& scale)
{
	Matrix4Intrinsics m;
	m.values[0]  = scale.x;
	m.values[5]  = scale.y;
	m.values[10] = scale.z;
	return m;
}

Matrix4Intrinsics Matrix4Intrinsics::Scale(const Vector3Simple& scale)
{
	Matrix4Intrinsics m;
	m.values[0]  = scale.x;
	m.values[5]  = scale.y;
	m.values[10] = scale.z;
	return m;
}

Matrix4Intrinsics Matrix4Intrinsics::Translation(const Vector3Intrinsics& translation)
{
	Matrix4Intrinsics m;
	m.values[12] = translation.x;
	m.values[13] = translation.y;
	m.values[14] = translation.z;
	return m;
}

Matrix4Intrinsics Matrix4Intrinsics::Translation(const Vector3Simple& translation)
{
	Matrix4Intrinsics m;
	m.values[12] = translation.x;
	m.values[13] = translation.y;
	m.values[14] = translation.z;
	return m;
}


std::ostream& operator<<(std::ostream& o, const Matrix4Intrinsics& m)
{
	return o << "Mat4(" << LINE_SEPARATOR <<
	       "\t" << m.values[0] << ", " << m.values[4] << ", " << m.values[8] << ", " << m.values [12] << ", " << LINE_SEPARATOR <<
	       "\t" << m.values[1] << ", " << m.values[5] << ", " << m.values[9] << ", " << m.values [13]  << ", " << LINE_SEPARATOR <<
	       "\t" << m.values[2] << ", " << m.values[6] << ", " << m.values[10] << ", " << m.values [14]  << ", " << LINE_SEPARATOR <<
	       "\t" << m.values[3] << ", " << m.values[7] << ", " << m.values[11] << ", " << m.values [15] << LINE_SEPARATOR <<
	       " )";
}