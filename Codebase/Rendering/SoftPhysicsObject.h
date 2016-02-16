#pragma once
#include "PhysicsObject.h"

class SoftPhysicsObject : public PhysicsObject
{
public:
	SoftPhysicsObject();
	~SoftPhysicsObject();
	virtual bool CreatePhysicsBody(double mass, const Vec3Physics& position, const QuatPhysics& orientation, const Vec3Physics& inertia = Vec3Physics(0, 0, 0), bool particle = false) override;
	virtual btSoftBody* GetPhysicsBody() override;

protected:
	btSoftBodyWorldInfo* m_SoftBodyInfo;
};