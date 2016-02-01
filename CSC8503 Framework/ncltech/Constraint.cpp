#include "Constraint.h"

Constraint::Constraint() {
	this->objA = NULL;
	this->objB = NULL;

	this->b = 0.0f;

	delta = 0.0f;
	softness = 0.0f;
	impulseSum = 0.0f;
	impulseSumMin = -FLT_MAX;
	impulseSumMax = FLT_MAX;
}

Constraint::Constraint(PhysicsObject* objA, PhysicsObject* objB,
                       const Vec3Physics& j1, const Vec3Physics& j2, const Vec3Physics& j3, const Vec3Physics& j4, float b) {
	this->objA = objA;
	this->objB = objB;

	this->j1 = j1;
	this->j2 = j2;
	this->j3 = j3;
	this->j4 = j4;
	this->b = b;

	delta = 0.0f;
	softness = 0.0f;
	impulseSum = 0.0f;
	impulseSumMin = -FLT_MAX;
	impulseSumMax = FLT_MAX;
}

void Constraint::ApplyImpulse() {
	delta = 0.0;

	//J * M(-1) * J(t)
	float constraint_mass = objA->GetInverseMass() * j1.Dot(j1) +
	                        j2.Dot(objA->GetInverseInertia() * j2) +
	                        objB->GetInverseMass() * j3.Dot(j3) +
	                        j4.Dot(objB->GetInverseInertia() * j4) + softness;

	if (constraint_mass > 0.00001f) {
		//JV
		float jv = j1.Dot(objA->GetLinearVelocity()) +
			j2.Dot(objA->GetAngularVelocity()) +
			j3.Dot(objB->GetLinearVelocity()) + 
			j4.Dot(objB->GetAngularVelocity());
		float denom = -(jv + b);
		delta = denom / constraint_mass;
	}

	float oldImpulseSum = impulseSum;
	impulseSum = std::min(std::max(impulseSum + delta, impulseSumMin), impulseSumMax);
	float realDelta = impulseSum - oldImpulseSum;

	objA->SetLinearVelocity(objA->GetLinearVelocity() + (j1 * realDelta) * objA->GetInverseMass());
	objA->SetAngularVelocity(objA->GetAngularVelocity() + objA->GetInverseInertia() * (j2 * realDelta));

	objB->SetLinearVelocity(objB->GetLinearVelocity() + (j3 * realDelta) * objB->GetInverseMass());
	objB->SetAngularVelocity(objB->GetAngularVelocity() + objB->GetInverseInertia() * (j4 * realDelta));
}