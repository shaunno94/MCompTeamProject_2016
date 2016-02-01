#include "Plane.h"

Plane::Plane(const Vec3Graphics& normal, float distance, bool normalise) {
	if(normalise) {
		float length = normal.Dot(normal);

		this->normal   = normal		/ length;
		this->distance = distance	/ length;
	} else {
		this->normal = normal;
		this->distance = distance;
	}
}

bool Plane::SphereInPlane(const Vec3Graphics& position, float radius) const {
	if(position.Dot(normal)+distance <= -radius)
		return false;

	return true;
}

bool Plane::PointInPlane(const Vec3Graphics& position) const {
	float test = position.Dot(normal);
	float test2 = test + distance;

	if(position.Dot(normal)+distance < -0.0001f)
		return false;

	return true;
}