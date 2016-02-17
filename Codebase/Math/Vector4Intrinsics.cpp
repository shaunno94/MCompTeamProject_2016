#ifndef ORBIS
#include "Vector4Intrinsics.h"

const Vector4Intrinsics Vector4Intrinsics::ZEROS = Vector4Intrinsics(_mm_set1_ps(0.0f));
const Vector4Intrinsics Vector4Intrinsics::ONES = Vector4Intrinsics(_mm_set1_ps(1.0f));
#endif