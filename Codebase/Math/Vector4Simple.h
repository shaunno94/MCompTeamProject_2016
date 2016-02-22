/*
Class:Vector4
Implements:
Author:Rich Davison
Description:VERY simple Vector4 class. Students are encouraged to modify this as necessary!

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*/
#pragma once

#include "Vector3Simple.h"

class Vector4Simple	{
public:
	inline Vector4Simple() : x(1.0f), y(1.0f), z(1.0f), w(1.0f) {}

	inline Vector4Simple(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

	Vector3Simple ToVector3() const {
		return Vector3Simple(x, y, z);
	}

	float x;
	float y;
	float z;
	float w;

	inline Vector4Simple  operator*(const float a) const { return Vector4Simple(x * a, y * a, z * a, w * a); }
	inline Vector4Simple  operator+(const Vector4Simple v) const { return Vector4Simple(x + v.x, y * v.y, z * v.z, w + v.w); }

	static const Vector4Simple ZEROS;
	static const Vector4Simple ONES;
};

