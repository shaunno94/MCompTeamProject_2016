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
	reset();
}


ControllerComponent::~ControllerComponent()
{
}

void ControllerComponent::updateObject(float dt)
{
	if (force.LengthSq() > 0.0000001 || torque.LengthSq() > 0.0000001 || impulse.LengthSq() > 0.0000001)
		dynamic_cast<RigidPhysicsObject*>(m_parent->GetPhysicsComponent())->GetPhysicsBody()->activate();

	Vec3 up = getOrientation() * Vec3(0, 1, 0);

	if (up.Dot(Vec3(0, 1, 0)) > 0.8 && !airbourne())
	{
		dynamic_cast<RigidPhysicsObject*>(m_parent->GetPhysicsComponent())->GetPhysicsBody()->applyCentralForce(btVector3(force.x, force.y, force.z) * dt);
		dynamic_cast<RigidPhysicsObject*>(m_parent->GetPhysicsComponent())->GetPhysicsBody()->applyCentralImpulse(btVector3(impulse.x, impulse.y, impulse.z));
		m_inactiveFramesUpsideDown = 0;
	}
	else if (up.Dot(Vec3(0, 1, 0)) < 0.5) {
		m_inactiveFramesUpsideDown++;
	}
	else {
		m_inactiveFramesUpsideDown = 0;
	}

	if (m_inactiveFramesUpsideDown > 120) {
		reset();
		m_inactiveFramesUpsideDown = 0;
	}

	force.ToZero();
	impulse.ToZero();

	dynamic_cast<RigidPhysicsObject*>(m_parent->GetPhysicsComponent())->GetPhysicsBody()->applyTorque(btVector3(torque.x, torque.y, torque.z)*dt);
	torque.ToZero();

	Vec3Physics left = (getOrientation() * Vec3Physics(-1, 0, 0)).Normalize();
	btVector3 btleft(left.x, left.y, left.z);
	Vec3Physics forward = (getOrientation() * Vec3Physics(0, 0, -1)).Normalize();
	btVector3 btforward(forward.x, forward.y, forward.z);
	btVector3 velocity = -m_parent->GetPhysicsComponent()->GetPhysicsBody()->getInterpolationLinearVelocity();
	btScalar friction = 0.5;

	if (velocity.length2() > 0.0000001)
	{
		friction = std::abs((2.0 * (velocity.normalize()).dot(btleft.normalize())));

		friction = friction <= 0.8 ? 0.8 : friction;


		/*float dot = velocity.normalize().dot(btforward.normalize());
		velocity = dynamic_cast<RigidPhysicsObject*>(m_parent->GetPhysicsComponent())->GetPhysicsBody()->getLinearVelocity();
		float angle = acos(dot);

		dynamic_cast<RigidPhysicsObject*>(m_parent->GetPhysicsComponent())->GetPhysicsBody()->setLinearVelocity(velocity * btMatrix3x3(btQuaternion(btVector3(0, 1, 0), angle)));
	*/}

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
void ControllerComponent::AddImpulse(float x, float y, float z)
{
	impulse.x = (x);
	impulse.y = (y);
	impulse.z = (z);
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
	btVector3 world = m_parent->GetPhysicsComponent()->GetPhysicsBody()->getWorldTransform().getOrigin();
	//world.SetTranslation(Vec3Physics(0, 0, 0));

	btVector3 worldNorm = world;
	worldNorm.normalize();

	float dot = worldNorm.dot(btVector3(0, 0, 1));

	float radians = std::acos(dot);

	if (world.x() < 0)
		radians = -radians;
	btTransform transform = btTransform(btQuaternion(btVector3(0, 1, 0), radians), world);
	m_parent->GetPhysicsComponent()->GetPhysicsBody()->setWorldTransform(transform);
	dynamic_cast<RigidPhysicsObject*>(m_parent->GetPhysicsComponent())->GetPhysicsBody()->activate();
}

void ControllerComponent::turnWheels(float prop)
{
	GameObject* fr = m_parent->FindGameObject("fr");
	GameObject* fl = m_parent->FindGameObject("fl");
	if (!(fr && fl))
	{
		return;
	}
	((WheelObject*)fr)->setRotationFactor(prop);
	((WheelObject*)fl)->setRotationFactor(prop);
}

bool ControllerComponent::airbourne()
{
	return  m_parent->GetPhysicsComponent()->GetPhysicsBody()->getWorldTransform().getOrigin().y() > 1.5;
}