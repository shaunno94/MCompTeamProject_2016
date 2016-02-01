#pragma once

#include "Math/nclglMath.h"

static inline float Lerp(float start, float end, float progress) {
	return progress * (end - start) + start;
};
static inline double Lerp(double start, double end, double progress) {
	return progress * (end - start) + start;
}
//static inline Vector2 Lerp(const Vector2& start, const Vector2& end, float progress) {
//	return Vector2(Lerp(start.x, end.x, progress), Lerp(start.y, end.y, progress));
//};
//static inline Vector3 Lerp(const Vector3& start, const Vector3& end, float progress) {
//	return Vector3(Lerp(start.x, end.x, progress), Lerp(start.y, end.y, progress), Lerp(start.z, end.z, progress));
//};
//static inline Vector4 Lerp(const Vector4& start, const Vector4& end, float progress) {
//	return Vector4(Lerp(start.x, end.x, progress), Lerp(start.y, end.y, progress), Lerp(start.z, end.z, progress), Lerp(start.w, end.w, progress));
//};

//http://en.wikipedia.org/wiki/Smooth
static inline float Smooth(float start, float end, float progress) {
	return Lerp(start, end, progress * progress * (3 - 2 * progress));
}
static inline double Smooth(double start, double end, double progress) {
	return Lerp(start, end, progress * progress * (3 - 2 * progress));
}
//static inline Vector2 Smooth(const Vector2& start, const Vector2& end, float progress) {
//	return Vector2(Smooth(start.x, end.x, progress), Smooth(start.y, end.y, progress));
//};
//static inline Vector3 Smooth(const Vector3& start, const Vector3& end, float progress) {
//	return Vector3(Smooth(start.x, end.x, progress), Smooth(start.y, end.y, progress), Smooth(start.z, end.z, progress));
//};
//static inline Vector4 Smooth(const Vector4& start, const Vector4& end, float progress) {
//	return Vector4(Smooth(start.x, end.x, progress), Smooth(start.y, end.y, progress), Smooth(start.z, end.z, progress), Smooth(start.w, end.w, progress));
//};

static inline float Smoother(float start, float end, float progress) {
	return Lerp(start, end, progress * progress * progress * (progress * (progress * 6 - 15) + 10));
}
static inline double Smoother(double start, double end, double progress) {
	return Lerp(start, end, progress * progress * progress * (progress * (progress * 6 - 15) + 10));
}
//static inline Vector2 Smoother(const Vector2& start, const Vector2& end, float progress) {
//	return Vector2(Smoother(start.x, end.x, progress), Smoother(start.y, end.y, progress));
//};
//static inline Vector3 Smoother(const Vector3& start, const Vector3& end, float progress) {
//	return Vector3(Smoother(start.x, end.x, progress), Smoother(start.y, end.y, progress), Smoother(start.z, end.z, progress));
//};
//static inline Vector4 Smoother(const Vector4& start, const Vector4& end, float progress) {
//	return Vector4(Smoother(start.x, end.x, progress), Smoother(start.y, end.y, progress), Smoother(start.z, end.z, progress), Smoother(start.w, end.w, progress));
//};