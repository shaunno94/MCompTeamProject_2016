#pragma once
/*
Class:Vector3
Implements:
Author:Rich Davison
Description:VERY simple Vector3 class. Students are encouraged to modify this as necessary!

-_-_-_-_-_-_-_,------,
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""

*/
#include <cmath>
#include <iostream>

class Vector3_1Intrinsics;

class Vector3Simple	{
 public:
	inline Vector3Simple() { ToZero(); }
	inline Vector3Simple(float x, float y, float z) : x(x), y(y), z(z) { }

	Vector3Simple(const Vector3_1Intrinsics& vec);

	float x;
	float y;
	float z;

	inline Vector3Simple& Invert() { x = -x; y = -y; z = -z; return *this;}

	inline Vector3Simple& Normalize() {
		float length = Length();
		if(length != 0.0f)	{
			length = 1.0f / length;
			x *= length;
			y *= length;
			z *= length;
		}
		return *this;
	}

	inline Vector3Simple Normal() const { return Vector3Simple(*this).Normalize(); }

	void ToZero() { x = y = z = 0.0f; }

	float Length() const { return sqrt((x * x) + (y * y) + (z * z)); }
	float LengthSq() const { return (x * x) + (y * y) + (z * z); }

	inline float Dot(const Vector3Simple& v) const { return (x * v.x) + (y * v.y) + (z * v.z); }

	inline Vector3Simple Cross(const Vector3Simple& v) const { return Vector3Simple((y * v.z) - (z * v.y) , (z * v.x) - (x * v.z) , (x * v.y) - (y * v.x)); }

	inline Vector3Simple  operator+(const Vector3Simple&  a) const { return Vector3Simple(x + a.x,y + a.y, z + a.z); }
	inline Vector3Simple  operator-(const Vector3Simple&  a) const { return Vector3Simple(x - a.x,y - a.y, z - a.z); }
	inline Vector3Simple  operator*(const Vector3Simple&  a) const { return Vector3Simple(x * a.x,y * a.y, z * a.z); }
	inline Vector3Simple  operator/(const Vector3Simple&  a) const { return Vector3Simple(x / a.x,y / a.y, z / a.z); };


	inline Vector3Simple& operator+=(const Vector3Simple&  a) { x += a.x; y += a.y; z += a.z; return *this; }
	inline Vector3Simple& operator-=(const Vector3Simple&  a) { x -= a.x; y -= a.y; z -= a.z; return *this; }


	inline Vector3Simple& operator+=(float a) { x += a; y += a; z += a; return *this; }
	inline Vector3Simple& operator-=(float a) { x -= a; y -= a; z -= a; return *this; }
	inline Vector3Simple& operator*=(float a) { x *= a; y *= a; z *= a; return *this; }
	inline Vector3Simple& operator/=(float a) { x /= a; y /= a; z /= a; return *this; }


	inline Vector3Simple  operator*(const float a) const { return Vector3Simple(x * a,y * a, z * a); }
	inline Vector3Simple  operator/(const float v) const { return Vector3Simple(x / v,y / v, z / v); };

	inline Vector3Simple operator-() const { return Vector3Simple(-x, -y, -z); }

	inline bool	operator==(const Vector3Simple& A)const {return (A.x == x && A.y == y && A.z == z) ? true : false;};
	inline bool	operator!=(const Vector3Simple& A)const {return (A.x == x && A.y == y && A.z == z) ? false : true;};


	static const Vector3Simple ZEROS;
	static const Vector3Simple ONES;

	inline friend std::ostream& operator<<(std::ostream& o, const Vector3Simple& v) {
		return o << "Vector3_1(" << v.x << ", " << v.y << ", " << v.z << ")";
	}
};

