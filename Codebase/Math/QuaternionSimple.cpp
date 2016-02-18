#include "QuaternionSimple.h"
#include "Helpers/degrees.h"
#include "Helpers/common.h"

const QuaternionSimple QuaternionSimple::EMPTY = QuaternionSimple(0.0f, 0.0f, 0.0f, 0.0f);
const QuaternionSimple QuaternionSimple::IDENTITY = QuaternionSimple(0.0f, 0.0f, 0.0f, 1.0f);


float QuaternionSimple::Dot(const QuaternionSimple& a,const QuaternionSimple& b) {
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
}


QuaternionSimple QuaternionSimple::operator *(const QuaternionSimple& b) const {
	QuaternionSimple ans;

	ans.w = (w * b.w) - (x * b.x) - (y * b.y) - (z * b.z);
	ans.x = (x * b.w) + (w * b.x) + (y * b.z) - (z * b.y);
	ans.y = (y * b.w) + (w * b.y) + (z * b.x) - (x * b.z);
	ans.z = (z * b.w) + (w * b.z) + (x * b.y) - (y * b.x);

	return ans;
}

QuaternionSimple QuaternionSimple::operator *(const Vector3Simple& b) const {
	/*QuaternionSimple ans;

	ans.w = -(x * b.x) - (y * b.y) - (z * b.z);
	ans.x = (w * b.x) + (y * b.z) - (z * b.y);
	ans.y = (w * b.y) + (z * b.x) - (x * b.z);
	ans.z = (w * b.z) + (x * b.y) - (y * b.x);

	return ans;
	*/
	//This (^) is equiv to q * b, where the below is equiv to b * q (needed for physics)
	QuaternionSimple ans;

	ans.w = -(x * b.x) - (y * b.y) - (z * b.z);

	ans.x = (w * b.x) + (b.y * z) - (b.z * y);
	ans.y = (w * b.y) + (b.z * x) - (b.x * z);
	ans.z = (w * b.z) + (b.x * y) - (b.y * x);

	return ans;
}

Matrix4Simple QuaternionSimple::ToMatrix4() const {
	Matrix4Simple mat;

	float yy = y*y;
	float zz = z*z;
	float xy = x*y;
	float zw = z*w;
	float xz = x*z;
	float yw = y*w;
	float xx = x*x;
	float yz = y*z;
	float xw = x*w;

	mat.values[0] = 1 - 2*yy - 2*zz;
	mat.values[1] = 2*xy + 2*zw;
	mat.values[2] = 2*xz - 2*yw;

	mat.values[4] = 2*xy - 2*zw;
	mat.values[5] = 1- 2*xx - 2*zz;
	mat.values[6] = 2*yz + 2*xw;

	mat.values[8] = 2*xz + 2*yw;
	mat.values[9] = 2*yz - 2*xw;
	mat.values[10] = 1 - 2*xx - 2*yy;

	return mat;
}

Matrix3Simple QuaternionSimple::ToMatrix3() const{
	Matrix3Simple mat;

	float yy = y*y;
	float zz = z*z;
	float xy = x*y;
	float zw = z*w;
	float xz = x*z;
	float yw = y*w;
	float xx = x*x;
	float yz = y*z;
	float xw = x*w;

	mat.values[0] = 1 - 2 * yy - 2 * zz;
	mat.values[1] = 2 * xy + 2 * zw;
	mat.values[2] = 2 * xz - 2 * yw;

	mat.values[3] = 2 * xy - 2 * zw;
	mat.values[4] = 1 - 2 * xx - 2 * zz;
	mat.values[5] = 2 * yz + 2 * xw;

	mat.values[6] = 2 * xz + 2 * yw;
	mat.values[7] = 2 * yz - 2 * xw;
	mat.values[8] = 1 - 2 * xx - 2 * yy;

	return mat;
}

QuaternionSimple QuaternionSimple::EulerAnglesToQuaternion(float pitch, float yaw, float roll)	{
	float y2 = (float)DegToRad(yaw/2.0f);
	float p2 = (float)DegToRad(pitch/2.0f);
	float r2 = (float)DegToRad(roll/2.0f);


	float cosy   = (float)cos(y2);
	float cosp   = (float)cos(p2);
	float cosr   = (float)cos(r2);

	float siny   = (float)sin(y2);
	float sinp   = (float)sin(p2);
	float sinr   = (float)sin(r2);

	QuaternionSimple q;


	q.x = cosr * sinp * cosy + sinr * cosp * siny;
	q.y = cosr * cosp * siny - sinr * sinp * cosy;
	q.z = sinr * cosp * cosy - cosr * sinp * siny;
	q.w = cosr * cosp * cosy + sinr * sinp * siny;

	return q;
};

QuaternionSimple QuaternionSimple::AxisAngleToQuaterion(const Vector3Simple& vector, float degrees)	{
	float theta = (float)DegToRad(degrees);
	float result = (float)sin(theta / 2.0f);

	return QuaternionSimple((float)(vector.x * result), (float)(vector.y * result), (float)(vector.z * result), (float)cos(theta / 2.0f));
}

void QuaternionSimple::GenerateW()	{
	w = 1.0f - (x*x)-(y*y)-(z*z);
	if(w < 0.0f)
		w = 0.0f;
	else
		w = - sqrt(w);
}

QuaternionSimple QuaternionSimple::FromMatrix(const Matrix4Simple& m)	{
	QuaternionSimple q;

	q.w = sqrt(std::max(0.0f, (1.0f + m.values[0] + m.values[5] + m.values[10]))) / 2.0f;
	q.x = sqrt(std::max(0.0f, (1.0f + m.values[0] - m.values[5] - m.values[10]))) / 2.0f;
	q.y = sqrt(std::max(0.0f, (1.0f - m.values[0] + m.values[5] - m.values[10]))) / 2.0f;
	q.z = sqrt(std::max(0.0f, (1.0f - m.values[0] - m.values[5] + m.values[10]))) / 2.0f;

	q.x = (float)copysignf(q.x, m.values[9] - m.values[6]);
	q.y = (float)copysignf(q.y, m.values[2] - m.values[8]);
	q.z = (float)copysignf(q.z, m.values[4] - m.values[1]);

	return q;
}

QuaternionSimple QuaternionSimple::Interpolate(const QuaternionSimple& pStart, const QuaternionSimple& pEnd, float pFactor)
{
	// calc cosine theta
	float cosom = pStart.x * pEnd.x + pStart.y * pEnd.y + pStart.z * pEnd.z + pStart.w * pEnd.w;

	// adjust signs (if necessary)
	QuaternionSimple end = pEnd;
	if (cosom < 0.0f)
	{
		cosom = -cosom;
		end.x = -end.x;   // Reverse all signs
		end.y = -end.y;
		end.z = -end.z;
		end.w = -end.w;
	}

	// Calculate coefficients
	float sclp, sclq;
	if ((1.0f - cosom) > 0.0001f) // 0.0001 -> some epsillon
	{
		// Standard case (slerp)
		float omega, sinom;
		omega = acos(cosom); // extract theta from dot product's cos theta
		sinom = sin(omega);
		sclp = sin((1.0f - pFactor) * omega) / sinom;
		sclq = sin(pFactor * omega) / sinom;
	}
	else
	{
		// Very close, do linear interp (because it's faster)
		sclp = 1.0f - pFactor;
		sclq = pFactor;
	}

	QuaternionSimple out;
	out.x = sclp * pStart.x + sclq * end.x;
	out.y = sclp * pStart.y + sclq * end.y;
	out.z = sclp * pStart.z + sclq * end.z;
	out.w = sclp * pStart.w + sclq * end.w;

	return out;
}