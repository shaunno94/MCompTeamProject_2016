#pragma once

#include "Vector2Simple.h"
#include "Vector3Simple.h"
#include "Vector4Simple.h"
#include "QuaternionSimple.h"
#include "Matrix3Simple.h"
#include "Matrix4Simple.h"

#include "Vector3Intrinsics.h"
#include "Vector3_1Intrinsics.h"
#include "Vector4Intrinsics.h"
#include "QuaternionIntrinsics.h"
#include "Matrix3Intrinsics.h"
#include "Matrix4Intrinsics.h"

//#define Qat QuaternionSimple
//#define QatGraphycs QuaternionSimple
//#define QatPhysics QuaternionSimple
//
//#define Vec2 Vector2Simple
//#define Vec2Graphycs Vector2Simple
//#define Vec2Physics Vector2Simple
//
//#define Vec3 Vector3Simple
//#define Vec3Graphycs Vector3Simple
//#define Vec3Physics Vector3Simple
//
//#define Vec4 Vector4Simple
//#define Vec4Graphycs Vector4Simple
//#define Vec4Physics Vector4Simple
//
//#define Mat3 Matrix3Simple
//#define Mat3Graphics Matrix3Simple
//#define Mat3Physics Matrix3Simple
//
//#define Mat4 Matrix4Simple
//#define Mat4Graphics Matrix4Simple
//#define Mat4Physics Matrix4Simple


typedef QuaternionSimple Quat;
typedef QuaternionSimple QuatGraphics;
typedef QuaternionSimple QuatPhysics;

typedef Vector2Simple Vec2;
typedef Vector2Simple Vec2Graphics;
typedef Vector2Simple Vec2Physics;

typedef Vector3Simple Vec3;
typedef Vector3Simple Vec3Graphics;
//typedef Vector3Simple Vec3Physics;
typedef Vector3_1Intrinsics Vec3Physics;

typedef Vector4Simple Vec4;
typedef Vector4Simple Vec4Graphics;
typedef Vector4Simple Vec4Physics;

typedef Matrix3Simple Mat3;
typedef Matrix3Simple Mat3Graphics;
typedef Matrix3Simple Mat3Physics;

typedef Matrix4Simple Mat4;
typedef Matrix4Simple Mat4Graphics;
typedef Matrix4Intrinsics Mat4Physics;
//typedef Matrix4Simple Mat4Physics;
