#include "ControllerComponent.h"
#include "GameObject.h"
#include "WheelObject.h"
#include "Renderer.h"
#include <algorithm>

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

void ControllerComponent::updateObject(float dt)
{
	if (force.LengthSq() > 0.0000001 || torque.LengthSq() > 0.0000001)
		dynamic_cast<RigidPhysicsObject*>(m_parent->GetPhysicsComponent())->GetPhysicsBody()->activate();

	dynamic_cast<RigidPhysicsObject*>(m_parent->GetPhysicsComponent())->GetPhysicsBody()->applyCentralForce(btVector3(force.x, force.y, force.z) * dt);
	force.ToZero();
	dynamic_cast<RigidPhysicsObject*>(m_parent->GetPhysicsComponent())->GetPhysicsBody()->applyTorque(btVector3(torque.x, torque.y, torque.z)*dt);
	//dynamic_cast<RigidPhysicsObject*>(m_parent->GetPhysicsComponent())->GetPhysicsBody()->applyCentralImpulse(btVector3(torque.x, torque.y, torque.z)*dt);
	torque.ToZero();
	Vec3Physics orientation = (getOrientation() * Vec3Physics(-1, 0, 0)).Normalize();
	btVector3 btOrientation(orientation.x, orientation.y, orientation.z);
	btVector3 velocity = -m_parent->GetPhysicsComponent()->GetPhysicsBody()->getInterpolationLinearVelocity();
	btScalar friction = 0.5;
	if (velocity.length2() > 0.0000001)
	{
		friction = std::abs((3 * (velocity.normalize()).dot(btOrientation.normalize())));
	
		friction = friction <= 0.5 ? 0.5 : friction;
	}
	m_parent->GetPhysicsComponent()->GetPhysicsBody()->setFriction(friction);
}

void ControllerComponent::AddForce(float x, float y, float z)
{
	force.x = (x);
	force.y = (y);
	force.z = (z);
}

void ControllerComponent::AddTorque(float x, float y, float z)
{
	torque.x = (x);
	torque.y = (y);
	torque.z = (z);
}

Mat4Physics ControllerComponent::getOrientation()
{
	//return Mat4Physics::Rotation(Renderer::GetInstance()->GetCurrentScene()->getCamera()->GetYaw() + 90, Vec3Physics(0, 1, 0));
	return m_parent->GetWorldTransform().GetRotation();
}

float ControllerComponent::getForwardVelocity()
{
	auto btvelocity = m_parent->GetPhysicsComponent()->GetPhysicsBody()->getInterpolationLinearVelocity();
	Vec3Physics velocity(btvelocity.x(), btvelocity.y(), btvelocity.z());
	auto forward = m_parent->GetWorldTransform().GetRotation() * Vec3Physics(0, 0, -1);
	forward.Normalize();
	return velocity.Dot(forward);
}

void ControllerComponent::setCameraControl(float pitch, float yaw)
{
	dPitch += pitch;
	dYaw += yaw;
}
void ControllerComponent::getCameraControl(float& pitch, float& yaw)
{
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

void ControllerComponent::reset()
{
	//return Mat4Physics::Rotation(Renderer::GetInstance()->GetCurrentScene()->getCamera()->GetYaw() + 90, Vec3Physics(0, 1, 0));
	auto world = m_parent->GetWorldTransform().GetTranslation();
	//world.SetTranslation(Vec3Physics(0, 0, 0));
	btTransform trasform = btTransform(btQuaternion(btVector3(0, 0, -1), 0), btVector3(world.x, world.y, world.z));
		m_parent->GetPhysicsComponent()->GetPhysicsBody()->setWorldTransform(trasform);
}

void ControllerComponent::turnWheels(float prop){
	GameObject* fr = m_parent->FindGameObject("fr");
	GameObject* fl = m_parent->FindGameObject("fl");
	if (!(fr && fl)){
		return;
	}
	((WheelObject*)fr)->setRotationFactor(prop);
	((WheelObject*)fl)->setRotationFactor(prop);
}