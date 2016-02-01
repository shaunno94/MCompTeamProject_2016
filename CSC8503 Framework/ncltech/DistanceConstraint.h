

#pragma once

#include "Constraint.h"
#include "NCLDebug.h"
#include "PhysicsEngine.h"

class DistanceConstraint : public Constraint
{
public:
	DistanceConstraint(PhysicsObject* objA, PhysicsObject* objB,
		const Vec3Physics& globalOnA, const Vec3Physics& globalOnB)
	{
		this->objA = objA;
		this->objB = objB;

		Vec3Physics ab = globalOnB - globalOnA;
		this->distance = ab.Length();

		Vec3Physics r1 = (globalOnA - objA->GetPosition());
		Vec3Physics r2 = (globalOnB - objB->GetPosition());
		localOnA = Mat3Physics::Transpose(objA->GetOrientation().ToMatrix3()) * r1;
		localOnB = Mat3Physics::Transpose(objB->GetOrientation().ToMatrix3()) * r2;

		delta = 0.0f;
		impulseSum = 0.0f;
		impulseSumMin = -FLT_MAX;
		impulseSumMax = FLT_MAX;
	}


	virtual void PreSolverStep(float dt)
	{
		Vec3Physics r1 = objA->GetOrientation().ToMatrix3() * localOnA;
		Vec3Physics r2 = objB->GetOrientation().ToMatrix3() * localOnB;

		Vec3Physics globalOnA = r1 + objA->GetPosition();
		Vec3Physics globalOnB = r2 + objB->GetPosition();

		Vec3Physics ab = globalOnB - globalOnA;
		Vec3Physics abn = ab.Normal();

		j1 = -abn;
		j2 = (-r1).Cross(abn);
		j3 = abn;
		j4 = r2.Cross(abn);

		/*
		Baumgarte Offset (Adds energy to the system to counter slight solving errors that accumulate over time - known as "constraint drift")
		*/
		{
			float distanceOffset = distance - ab.Length();
			float baumgarteScaler = 0.1f;
			b = -(baumgarteScaler / PhysicsEngine::Instance()->GetDeltaTime()) * distanceOffset;
		}
	}

	virtual void DebugDraw() const
	{
		Vec3Physics globalOnA = objA->GetOrientation().ToMatrix3() * localOnA + objA->GetPosition();
		Vec3Physics globalOnB = objB->GetOrientation().ToMatrix3() * localOnB + objB->GetPosition();

		NCLDebug::DrawThickLine(globalOnA, globalOnB, 0.02f, Vec4Graphics(0.0f, 0.0f, 0.0f, 1.0f));
		NCLDebug::DrawPoint(globalOnA, 0.05f, Vec4Graphics(1.0f, 0.8f, 1.0f, 1.0f));
		NCLDebug::DrawPoint(globalOnB, 0.05f, Vec4Graphics(1.0f, 0.8f, 1.0f, 1.0f));
	}

protected:
	float   distance;
	Vec3Physics localOnA, localOnB;
};