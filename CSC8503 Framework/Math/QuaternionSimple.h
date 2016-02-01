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
#include "Matrix4Simple.h"
#include "Matrix3Simple.h"

class Matrix4Simple;

class QuaternionSimple	{
 public:
	inline QuaternionSimple() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) {}
	inline QuaternionSimple(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

	float x;
	float y;
	float z;
	float w;

	inline QuaternionSimple& Normalize() {
		float magnitude = sqrt(Dot(*this,*this));
		if(magnitude > 0.0f) {
			float t= 1.0f / magnitude;
			x *= t;
			y *= t;
			z *= t;
			w *= t;
		}
		return *this;
	}

	inline QuaternionSimple Normal() { return QuaternionSimple(*this).Normalize(); }

	Matrix4Simple ToMatrix4() const;
	Matrix3Simple ToMatrix3() const;

	inline QuaternionSimple Conjugate() const { return QuaternionSimple(-x,-y,-z,w); }
	void		GenerateW();	//builds 4th component when loading in shortened, 3 component quaternions

	static const QuaternionSimple EMPTY;
	static const QuaternionSimple IDENTITY;

	static QuaternionSimple EulerAnglesToQuaternion(float pitch, float yaw, float roll);
	static QuaternionSimple AxisAngleToQuaterion(const Vector3Simple& vector, float degrees);

	static void RotatePointByQuaternion(const QuaternionSimple& q, Vector3Simple& point);

	static QuaternionSimple FromMatrix(const Matrix4Simple& m);

	static float Dot(const QuaternionSimple& a, const QuaternionSimple& b);

	QuaternionSimple operator *(const QuaternionSimple& a) const;
	QuaternionSimple operator *(const Vector3Simple& a) const;

	inline QuaternionSimple operator+(const QuaternionSimple &a) const {
		return QuaternionSimple(x + a.x, y + a.y, z + a.z, w + a.w);
	}

	QuaternionSimple Interpolate(const QuaternionSimple& pStart, const QuaternionSimple& pEnd, float pFactor);

	inline friend std::ostream& operator<<(std::ostream& o, const QuaternionSimple& q) {
		return o << "Quat(" << q.x << "," << q.y << "," << q.z <<  "," << q.w << ")";
	}
};