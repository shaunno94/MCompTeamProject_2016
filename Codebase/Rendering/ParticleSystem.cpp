#include "ParticleSystem.h"
#include "Scene.h"
#include "./Helpers/RNG.h"

ParticleSystem::ParticleSystem(Material* material, GameObject* attatchment, Scene* scene, unsigned int maxParticles)
{
	m_Material = material;
	m_Scene = scene;
	m_Attatchment = attatchment;
	m_NumParticles = maxParticles;
	m_Particles.resize(maxParticles);
	
	for (int i = 0; i < maxParticles; i++)
	{
		m_Particles[i] = new Particle(attatchment, Matrix4Simple::Scale(m_Scale) * Matrix4Simple::Translation(m_Offset));
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
	m_Particles.clear();
}

void ParticleSystem::EmitParticle(Particle* particle, float dt)
{
	float x = RNG32::Rand(MIN_FORCE_XZ, MAX_FORCE_XZ);
	float z = RNG32::Rand(MIN_FORCE_XZ, MAX_FORCE_XZ);

	btVector3 vel = static_cast<RigidPhysicsObject*>(m_Attatchment->GetPhysicsComponent())->GetPhysicsBody()->getLinearVelocity();
	btVector3 velN = vel;
	velN += btVector3(15, 0.00001, 15);
	float speed = fmin((velN.length2() / 100000.0f), 30.0f);
	particle->SetVelocity(particle->GetVelocity() + (Vec3Physics(x * speed, 0.1, z * speed) ));
	particle->SetOldPosition(particle->GetPosition());
	particle->SetPosition(particle->GetPosition() + (particle->GetVelocity() * (dt / 1000.0f)));
}

void ParticleSystem::Update(float delta)
{ 
	for (unsigned int i = 0; i < m_Particles.size(); ++i)
	{
		m_Particles[i]->SetAge(m_Particles[i]->GetAge() + delta);

		if (m_Particles[i]->GetAge() < m_Particles[i]->GetLife())
		{
			EmitParticle(m_Particles[i], delta);
			m_Particles[i]->spawned = true;
		}
		else
		{
			m_Particles[i]->SetLocalTransform(Matrix4Simple::Scale(m_Scale) * Matrix4Simple::Translation(m_Offset));
			m_Particles[i]->SetVelocity(Vec3Physics(0, 0, 0));
			m_Particles[i]->SetOldPosition(Vec3Physics(0, 0, 0));
			m_Particles[i]->SetPosition(Vec3Physics(0, 0, 0));
			m_Particles[i]->SetAge(0);
			m_Particles[i]->spawned = false;
			m_Particles[i]->carGhostLoc = m_Attatchment->GetWorldTransform();
		}
	}
}