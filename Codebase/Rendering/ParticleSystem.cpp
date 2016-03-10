#include "ParticleSystem.h"
#include "Scene.h"
#include "./Helpers/RNG.h"

ParticleSystem::ParticleSystem(ParticleEmitter* emitter, Material* material, GameObject* attatchment, Scene* scene, unsigned int maxParticles)
{
	m_Material = material;
	m_Scene = scene;
	m_Attatchment = attatchment;
	m_Force = Vector3Simple(0, -9.81f, 0);
	m_NumAlive = 0;
	m_ParticleEmitter = emitter;
	m_NumParticles = maxParticles;
	m_Particles.resize(maxParticles);
	transform.setOrigin(btVector3(0.0, 0.0, 0.0));
	
	for (int i = 0; i < maxParticles; i++)
	{
		m_Particles[i] = new Particle(attatchment);
		m_Particles[i]->SetPhysicsComponent(new RigidPhysicsObject());
		m_Particles[i]->GetPhysicsComponent()->CreateCollisionShape(0.1);
		m_Particles[i]->GetPhysicsComponent()->CreatePhysicsBody(1.0, Vec3Physics(0, 0, 0), QuatPhysics(0, 0, 0, 1), Vec3Physics(0, 0, 0), true);
		m_Particles[i]->SetRenderComponent(new RenderComponent(m_Material, Mesh::GenerateQuad()));
		m_Scene->addParticleObject(m_Particles[i]);
	}
}

ParticleSystem::~ParticleSystem()
{
	for (auto& particles : m_Particles)
	{
		if (particles)
		{
			delete particles;
			particles = nullptr;
		}
	}
}

void ParticleSystem::EmitParticle(Particle* particle, float dt)
{
	float x = RNG32::Rand(MIN_FORCE_XZ, MAX_FORCE_XZ);
	float y = RNG32::Rand(MIN_FORCE_Y, MAX_FORCE_Y);
	float z = RNG32::Rand(MIN_FORCE_XZ, MAX_FORCE_XZ);
	static_cast<RigidPhysicsObject*>(particle->GetPhysicsComponent())->GetPhysicsBody()->applyCentralImpulse(btVector3(x, y, z));
}

void ParticleSystem::Update(float delta)
{
	for (unsigned int i = 0; i < m_Particles.size(); ++i)
	{
		m_Particles[i]->SetAge(m_Particles[i]->GetAge() + delta);

		if (m_Particles[i]->GetAge() < m_Particles[i]->GetLife())
		{
			EmitParticle(m_Particles[i], delta);
		}
		else
		{
			static_cast<RigidPhysicsObject*>(m_Particles[i]->GetPhysicsComponent())->GetPhysicsBody()->setLinearVelocity(btVector3(0, 0, 0));
			m_Particles[i]->GetPhysicsComponent()->GetPhysicsBody()->setWorldTransform(transform);
			m_Particles[i]->SetAge(0);
		}
	}
}