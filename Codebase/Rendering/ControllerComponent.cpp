#include "ControllerComponent.h"
#include "GameObject.h"
#include "Renderer.h"

ControllerComponent::ControllerComponent(GameObject* parent)
{
	m_parent = parent;
	m_parent->SetControllerComponent(this);
	force.ToZero();
	torque.ToZero();
	dPitch = 0;
	dYaw = 0;
}


ControllerComponent::~ControllerComponent()
{
}

void ControllerComponent::updateObject(float dt){
	if (force.Length() > 0.0000001 || torque.Length() > 0.0000001)
		dynamic_cast<RigidPhysicsObject*>(m_parent->GetPhysicsComponent())->GetPhysicsBody()->activate();
	dynamic_cast<RigidPhysicsObject*>(m_parent->GetPhysicsComponent())->GetPhysicsBody()->applyCentralForce(btVector3(force.x, force.y, force.z) * dt);
	force.ToZero();
	dynamic_cast<RigidPhysicsObject*>(m_parent->GetPhysicsComponent())->GetPhysicsBody()->applyTorque(btVector3(torque.x, torque.y, torque.z)*dt);
	//dynamic_cast<RigidPhysicsObject*>(m_parent->GetPhysicsComponent())->GetPhysicsBody()->applyCentralImpulse(btVector3(torque.x, torque.y, torque.z)*dt);
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
	//Renderer::GetInstance()->GetCurrentScene()->getCamera();
	return m_parent->GetWorldTransform().GetRotation();
}

void ControllerComponent::setCameraControl(float pitch, float yaw){
	dPitch += pitch;
	dYaw += yaw;
}
void ControllerComponent::getCameraControl(float& pitch, float& yaw){
	pitch -= dPitch;
	yaw -= dYaw;
	dPitch = dYaw = 0;

	//Bounds check the pitch, to be between straight up and straight down ;)
	pitch = fmin(pitch, 45.0f);
	pitch = fmax(pitch, -45.0f);


	if (yaw < 0)
		yaw += 360.0f;
	if (yaw > 360.0f)
		yaw -= 360.0f;
}