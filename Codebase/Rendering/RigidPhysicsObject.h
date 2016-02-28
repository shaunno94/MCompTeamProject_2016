#pragma once
#include "PhysicsObject.h"

class RigidPhysicsObject : public PhysicsObject
{
public:
	RigidPhysicsObject();
	~RigidPhysicsObject();
	bool CreatePhysicsBody(double mass, const Vec3Physics& position, const QuatPhysics& orientation, const Vec3Physics& inertia = Vec3Physics(0, 0, 0), bool particle = false) override;
	btRigidBody* GetPhysicsBody() override;

protected:
	btRigidBody::btRigidBodyConstructionInfo* m_ConstructionInfo;
	btDefaultMotionState*		m_MotionState;
};