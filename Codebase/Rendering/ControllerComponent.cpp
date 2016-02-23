#include "ControllerComponent.h"
#include "GameObject.h"

ControllerComponent::ControllerComponent(GameObject* parent)
{
	m_parent = parent;
	m_parent->SetControllerComponent(this);
	force.ToZero();
	torque.ToZero();
}


ControllerComponent::~ControllerComponent()
{
}

void ControllerComponent::updateObject(float dt){
	if (force.Length() > 0.0000001)
		dynamic_cast<RigidPhysicsObject*>(m_parent->GetPhysicsComponent())->GetPhysicsBody()->activate();
	dynamic_cast<RigidPhysicsObject*>(m_parent->GetPhysicsComponent())->GetPhysicsBody()->applyCentralForce(btVector3(force.x, force.y, force.z) * dt);
	force.ToZero();
	dynamic_cast<RigidPhysicsObject*>(m_parent->GetPhysicsComponent())->GetPhysicsBody()->applyTorque(btVector3(force.x, force.y, force.z)*dt);
	torque.ToZero();
}

void ControllerComponent::AddForce(float x, float y, float z){
	force.x = (x);
	force.y = (y);
	force.z = (z);
}

void ControllerComponent::AddTorque(float x, float y, float z){
	torque.x = (x);
	torque.y = (y);
	torque.z = (z);
}

Mat4Physics ControllerComponent::getOrientation(){
	return m_parent->GetWorldTransform().GetRotation();
}