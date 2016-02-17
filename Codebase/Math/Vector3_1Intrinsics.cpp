#ifndef ORBIS
#include "Vector3_1Intrinsics.h"

#include "Vector3Simple.h"

const Vector3_1Intrinsics Vector3_1Intrinsics::ZEROS = Vector3_1Intrinsics(0.0f, 0.0f, 0.0f);
const Vector3_1Intrinsics Vector3_1Intrinsics::ONES = Vector3_1Intrinsics(1.0f, 1.0f, 1.0f);


Vector3_1Intrinsics::Vector3_1Intrinsics(const Vector3Simple& v) : mmvalue(_mm_set_ps(0, v.z, v.y, v.x)) {
}
#endif