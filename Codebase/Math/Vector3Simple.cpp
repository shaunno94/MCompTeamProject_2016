#include "Vector3Simple.h"

#include "Vector3_1Intrinsics.h"

const Vector3Simple Vector3Simple::ZEROS = Vector3Simple(0.0f, 0.0f, 0.0f);
const Vector3Simple Vector3Simple::ONES = Vector3Simple(1.0f, 1.0f, 1.0f);

Vector3Simple::Vector3Simple(const Vector3_1Intrinsics& vec) {
	x = vec.x;
	y = vec.y;
	z = vec.z;
}