#include "WheelObject.h"


WheelObject::WheelObject(const std::string& name)
{
}


WheelObject::~WheelObject()
{
}

void WheelObject::OnUpdateObject(float dt)
{
	auto btvelocity = m_Parent->GetPhysicsComponent()->GetPhysicsBody()->getInterpolationLinearVelocity();
	Vec3Physics velocity(btvelocity.x(), btvelocity.y(), btvelocity.z());
	//velocity.Normalize();
	auto forward = m_Parent->GetWorldTransform().GetRotation() * Vec3Physics(0,0,1);
	forward.Normalize();
	float dot = velocity.Dot(forward);
	m_LocalTransform = m_LocalTransform * Mat4Physics::RotationX(dot*0.5);

	GameObject::OnUpdateObject(dt);
}