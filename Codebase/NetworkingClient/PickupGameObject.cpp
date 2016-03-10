#include "PickupGameObject.h"
#include <cmath>

PickupGameObject::PickupGameObject(const std::string& name, float radius, const Vec3Physics& pos) : GameObject(name)
{
	auto physicsComponent = new RigidPhysicsObject();
	physicsComponent->CreateCollisionShape(radius);
	physicsComponent->CreatePhysicsBody(0.0, pos, QuatPhysics::IDENTITY, Vec3Physics::ONES, true);
	SetPhysicsComponent(physicsComponent);
	auto broadphaseProxy = physicsComponent->GetPhysicsBody()->getBroadphaseProxy();
	broadphaseProxy->m_collisionFilterGroup = COL_GROUP_DEFAULT;
	broadphaseProxy->m_collisionFilterMask = COL_PICKUP;
	m_proxyId = broadphaseProxy->getUid();
	m_disabled = false;

	m_initialPos = GetLocalTransform().GetTranslation();
}


PickupGameObject::~PickupGameObject()
{
}


void PickupGameObject::Disable(float millisec)
{
	if (!m_disabled)
	{
		m_disabled = true;
		m_timeoutMillisec = millisec;
		m_timer.Get();
		m_lastActiveTransform = GetPhysicsComponent()->GetPhysicsBody()->getWorldTransform();
		btTransform temp = m_lastActiveTransform;
		temp.setOrigin(btVector3(0.0f, -1000.0f, 0.0f));
		GetPhysicsComponent()->GetPhysicsBody()->setWorldTransform(temp);
		GetRenderComponent()->disabled = true;
	}
}
void PickupGameObject::Enable()
{
	if (m_disabled)
	{
		m_disabled = false;
		GetPhysicsComponent()->GetPhysicsBody()->setWorldTransform(m_lastActiveTransform);
		GetRenderComponent()->disabled = false;
	}
}


void PickupGameObject::OnUpdateObject(float dt)
{
	if (m_disabled)
	{
		if (m_timer.GetMin(m_timeoutMillisec, 1000) > 0.0f)
		{
			Enable();
		}
	}

	const float rotationSpeed = 0.13f;
	const float upDownSpeed = 2.0f;
	Mat4Graphics newTransform = Mat4Graphics::RotationY(fmod(dt * rotationSpeed, 360)) * GetLocalTransform();
	newTransform.SetTranslation(m_initialPos + Vec3Graphics(0.0f, sin(fmod(m_timer.Age() * upDownSpeed, 6.28319)), 0.0f));
	SetLocalTransform(newTransform);

	GameObject::OnUpdateObject(dt);
}