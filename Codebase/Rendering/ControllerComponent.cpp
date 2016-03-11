#include "ControllerComponent.h"
#include "GameObject.h"
#include "WheelObject.h"
#include "Renderer.h"
#include <algorithm>
#include "Application\GameScene.h"

ControllerComponent::ControllerComponent(GameObject* parent)
{
	m_parent = parent;
	m_parent->SetControllerComponent(this);
	force.ToZero();
	torque.ToZero();
	dPitch = 0;
	dYaw = 0;
	reset();
	m_updateState = false;
	boost = boosting = 0;
}


ControllerComponent::~ControllerComponent()
{
}

void ControllerComponent::updateObject(float dt)
{
	//networking
	if (m_updateState)
	{
		auto rigid = static_cast<RigidPhysicsObject*>(m_parent->GetPhysicsComponent())->GetPhysicsBody();
		btVector3 oldOrigin = rigid->getWorldTransform().getOrigin();
		btVector3 newOrigin = btVector3(m_position.x, m_position.y, m_position.z);
		btVector3 originDiff = oldOrigin - newOrigin;

		if (m_linearVelocity.LengthSq() > 0.00001f || m_angularVelocity.LengthSq() > 0.00001f || originDiff.length2() > 0.00001f)
		{
			rigid->activate(true);
			btTransform newWorldTrans;
			newWorldTrans.setOrigin(newOrigin);
			newWorldTrans.setRotation(btQuaternion(m_orientation.x, m_orientation.y, m_orientation.z, m_orientation.w));
			rigid->setWorldTransform(newWorldTrans);
			//TODO: check if MotionState update is needed?
			rigid->getMotionState()->setWorldTransform(newWorldTrans);

			rigid->setLinearVelocity(btVector3(m_linearVelocity.x, m_linearVelocity.y, m_linearVelocity.z));
			rigid->setAngularVelocity(btVector3(m_angularVelocity.x, m_angularVelocity.y, m_angularVelocity.z));
		}

		m_updateState = false;
	}

	//wake up object
	if (force.LengthSq() > 0.0000001 || torque.LengthSq() > 0.0000001 || impulse.LengthSq() > 0.0000001)
		dynamic_cast<RigidPhysicsObject*>(m_parent->GetPhysicsComponent())->GetPhysicsBody()->activate();

	Vec3 up = getOrientation() * Vec3(0, 1, 0);

	//apply forces
	if (up.Dot(Vec3(0, 1, 0)) > 0.8 && !airbourne())
	{
		dynamic_cast<RigidPhysicsObject*>(m_parent->GetPhysicsComponent())->GetPhysicsBody()->applyCentralForce(btVector3(force.x, force.y, force.z) * dt);
		dynamic_cast<RigidPhysicsObject*>(m_parent->GetPhysicsComponent())->GetPhysicsBody()->applyCentralImpulse(btVector3(impulse.x, impulse.y, impulse.z));
		m_inactiveFramesUpsideDown = 0;
	}
	//auto reset
	else if (up.Dot(Vec3(0, 1, 0)) < 0.5) {
		m_inactiveFramesUpsideDown++;
	}
	else
	{
		m_inactiveFramesUpsideDown = 0;
	}

	if (m_inactiveFramesUpsideDown > 120)
	{
		reset();
		m_inactiveFramesUpsideDown = 0;
	}

	force.ToZero();
	impulse.ToZero();

	float maxSpeed = 100;

	//adjust torque relative to speed
	if (!airbourne() && torque.LengthSq() > 0)
	{
		float forwardVelocity = getForwardVelocity();
		torque *= ((1 - (forwardVelocity / maxSpeed)) * 100000) + 80000;
	}

	//apply torque
	dynamic_cast<RigidPhysicsObject*>(m_parent->GetPhysicsComponent())->GetPhysicsBody()->applyTorque(btVector3(torque.x, torque.y, torque.z)*dt);
	torque.ToZero();

	//vary friction relative to direction
	Vec3Physics left = (getOrientation() * Vec3Physics(-1, 0, 0)).Normalize();
	btVector3 btleft(left.x, left.y, left.z);
	btVector3 velocity = -m_parent->GetPhysicsComponent()->GetPhysicsBody()->getInterpolationLinearVelocity();

	btVector3 fullVelocity = dynamic_cast<RigidPhysicsObject*>(m_parent->GetPhysicsComponent())->GetPhysicsBody()->getLinearVelocity();

	btScalar friction = 0.8;

	if (velocity.length2() > 0.0000001)
	{
		float leftDot = (velocity.normalize()).dot(btleft.normalize());
		friction = std::abs((2.0 * leftDot));
		if (dynamic_cast<GameScene*>(Renderer::GetInstance()->GetCurrentScene())->GetGoalScored() == 0)
		{
			//clamp speed
			float velocityFactor = (maxSpeed * maxSpeed) / std::max(fullVelocity.length2(), maxSpeed * maxSpeed);
			dynamic_cast<RigidPhysicsObject*>(m_parent->GetPhysicsComponent())->GetPhysicsBody()->setLinearVelocity(fullVelocity * velocityFactor);
		}
		friction = friction <= 1 ? 1 : friction;

		//adjust for rotation
		if (!airbourne()){
			float angle = leftDot * 1.5708 * 0.02;
				if (getForwardVelocity() < 0)
				angle = -angle;
			dynamic_cast<RigidPhysicsObject*>(m_parent->GetPhysicsComponent())->GetPhysicsBody()->setLinearVelocity(fullVelocity * btMatrix3x3(btQuaternion(btVector3(0, 1, 0), -angle)));
		}
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
void ControllerComponent::AddImpulse(float x, float y, float z)
{
	impulse.x = (x);
	impulse.y = (y);
	impulse.z = (z);
}

void ControllerComponent::SetState(const Vec3Physics& pos, const QuatPhysics& orientation, const Vec3Physics& linearVelocity, const Vec3Physics& angularVelocity)
{
	m_position = pos;
	m_orientation = orientation;
	m_linearVelocity = linearVelocity;
	m_angularVelocity = angularVelocity;
	m_updateState = true;
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
	if (m_parent->GetPhysicsComponent())
	{
	//return Mat4Physics::Rotation(Renderer::GetInstance()->GetCurrentScene()->getCamera()->GetYaw() + 90, Vec3Physics(0, 1, 0));
	btVector3 world = m_parent->GetPhysicsComponent()->GetPhysicsBody()->getWorldTransform().getOrigin();
	//world.SetTranslation(Vec3Physics(0, 0, 0));

	btVector3 worldNorm = world;
	if (world.x() == 0.0f && world.y() == 0.0f && world.z() == 0.0f)
	{
		worldNorm = btVector3(0, 0, 1);
	}
	worldNorm.normalize();

	float dot = worldNorm.dot(btVector3(0, 0, 1));

	float radians = std::acos(dot);

	if (world.x() < 0)
		radians = -radians;
	btTransform transform = btTransform(btQuaternion(btVector3(0, 1, 0), radians), world);
	m_parent->GetPhysicsComponent()->GetPhysicsBody()->setWorldTransform(transform);
	dynamic_cast<RigidPhysicsObject*>(m_parent->GetPhysicsComponent())->GetPhysicsBody()->activate();

	}
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