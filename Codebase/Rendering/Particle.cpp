#include "Particle.h"
#include "./Helpers/RNG.h"

Particle::Particle(GameObject* parent, const std::string& name) : GameObject(name)
{
	m_Position = Vec3Graphics(0, 0, 0);
	m_Velocity = Vec3Graphics(0, 0, 0);
	m_Life = RNG32::Rand(MIN_LIFE, MAX_LIFE);
	m_Size = 0;
	m_Age = 0;
	m_LocalTransform = Matrix4Simple::Scale(m_Scale);
	m_Parent = parent;
}

void Particle::OnRenderObject()
{
	GameObject::OnRenderObject();
}

void Particle::OnUpdateObject(float dt)
{
	UpdateTransform();
}