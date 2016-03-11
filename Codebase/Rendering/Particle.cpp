#include "Particle.h"
#include "./Helpers/RNG.h"

Particle::Particle(GameObject* parent, Matrix4Simple initialTransform, const std::string& name) : GameObject(name)
{
	m_Position = Vec3Graphics(0, 0, 0);
	m_Velocity = Vec3Graphics(0, 0, 0);
	m_Life = RNG32::Rand(MIN_LIFE, MAX_LIFE);
	m_Age = 0;
	m_Parent = parent;
	m_LocalTransform = initialTransform;
}

void Particle::OnRenderObject()
{
	GameObject::OnRenderObject();
}

void Particle::OnUpdateObject(float dt)
{
	m_LocalTransform = Matrix4Simple::Translation(m_Position - m_OldPos) * m_LocalTransform;

	if (!spawned)
		m_WorldTransform = m_Parent->GetWorldTransform() * m_LocalTransform;
	else
		m_WorldTransform = carGhostLoc * m_LocalTransform;
}