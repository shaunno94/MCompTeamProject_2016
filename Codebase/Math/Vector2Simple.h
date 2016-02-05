#pragma once
/*
Class:Vector2
Implements:
Author:Rich Davison
Description:VERY simple Vector2 class. Students are encouraged to modify this as necessary!

-_-_-_-_-_-_-_,------,
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""

*/

#include <cmath>
#include <iostream>

class Vector2Simple	{
 public:
	inline Vector2Simple() : x(0.0f), y(0.0f) {}

	inline Vector2Simple(float x, float y) : x(x), y(y) {}

	float x;
	float y;

	inline Vector2Simple& ToZero() { x = y = 0.0f; return *this; }

	inline float Length() const { return sqrtf((x * x) + (y * y)); }

	inline float LengthSq() const { return (x * x) + (y * y); }

	inline Vector2Simple& Normalise() {
		float temp = LengthSq();
		if(temp != 0.0f && fabsf(1.0f - temp) > 0.0001f) {
			temp = 1.0f / sqrtf(temp);
			x *= temp;
			y *= temp;
		}
		return *this;
	}

	inline Vector2Simple& Invert() { x = -x; y = -y; return *this; }

	inline Vector2Simple Inverse() const { return Vector2Simple(-x, -y); }

	inline Vector2Simple operator+(const Vector2Simple& other) const { return Vector2Simple(x + other.x, y + other.y); }

	inline Vector2Simple operator-(const Vector2Simple& other) const { return Vector2Simple(x - other.x, y - other.y); }

	inline Vector2Simple operator*(float scaler) const { return Vector2Simple(x * scaler, y * scaler); }

	inline Vector2Simple  operator/(float scaler) const {
		if(scaler != 0.0f) {
			float inverseScaler = 1.0f / scaler;
			return Vector2Simple(x * inverseScaler, y * inverseScaler);
		}
		return *this;
	};

	inline Vector2Simple operator-() const { return Vector2Simple(-x, -y); }

	inline Vector2Simple& operator+=(const Vector2Simple&  other) { x += other.x; y += other.y; return *this; }

	inline Vector2Simple& operator-=(const Vector2Simple&  other) { x -= other.x; y -= other.y; return *this; }

	inline Vector2Simple& operator*=(float scaler) { x *= scaler; y *= scaler; return *this; }

	inline Vector2Simple& operator/=(float scaler) {
		if(scaler != 0.0f) {
			float reciprocalScaler = 1.0f / scaler;
			x *= reciprocalScaler;
			y *= reciprocalScaler;
		}
		return *this;
	}

	inline bool	operator==(const Vector2Simple& other) const { return other.x == x && other.y == y; };
	inline bool	operator!=(const Vector2Simple& other) const { return other.x != x || other.y != y; };

	static const Vector2Simple ZEROS;
	static const Vector2Simple ONES;

	friend std::ostream& operator<<(std::ostream& o, const Vector2Simple& v) {
		return o << "Vector2(" << v.x << ", " << v.y << ")";
	}
};

