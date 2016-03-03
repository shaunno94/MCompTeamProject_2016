#ifndef ORBIS
#include "ParticleSystem.h"

ParticleSystem::ParticleSystem(ParticleEmitter* emitter, Material* material, Texture* texture, unsigned int numParticles)
: m_Material(material)
	, m_Texture(texture)
	, m_Force(0,-9.81f,0)
	, m_NumAlive(0)
	, m_ParticleEmitter(emitter)
	, m_NumParticles(numParticles)
{
	m_Particles[numParticles];
	material->Set("diffuseTex", m_Texture);

	for (int i = 0; i < numParticles; i++)
		m_Particles->SetRenderComponent(new RenderComponent(m_Material, m_Mesh));
	
}

ParticleSystem::~ParticleSystem()
{

}

void ParticleSystem::EmitParticle(Particle& particle)
{
	if (m_ParticleEmitter)
		m_ParticleEmitter->EmitParticle(particle);
	else
	{
		//some form of default ways to emit particles
	}
}

void ParticleSystem::EmitParticles()
{
	for (unsigned int i = 0; i < m_NumParticles; ++i)
	{
		EmitParticle(m_Particles[i]);
	}
}

void ParticleSystem::BuildVertexBuffer()
{
	Vec3Graphics x = Vec3Graphics(0.5, 0.0, 0.0);
	Vec3Graphics y = Vec3Graphics(0.0, 0.5, 0.0);
	Vec3Graphics z = Vec3Graphics(0.0, 0.0, 1.0);

	for (unsigned int i = 0; i < m_NumParticles; ++i)
	{
		Particle& particle = m_Particles[i];

		unsigned int vertexIndex = i * 4;
		
		m_Centre[i] = Vec3Graphics((particle.m_Position + (-x * -y) * particle.m_Size).x,
			(particle.m_Position + (-x * -y) * particle.m_Size).y,
			(particle.m_Position + (-x * -y) * particle.m_Size).z);
	}
}

bool ParticleSystem::Update(float delta)
{
	for (unsigned int i = 0; i < m_NumParticles; ++i)
	{
		Particle& particle = m_Particles[i];

		particle.m_Age += delta;
		if (particle.m_Age > particle.m_Life)
			EmitParticle(particle);

		float lifeRatio = ClampValues((particle.m_Age / particle.m_Life), 0.0f, 1.0f);
		particle.m_Velocity += (m_Force * delta);
		particle.m_Position += (particle.m_Velocity * delta);
		particle.m_Colour = Vec4Graphics(1.0f, 0.0f, 0.0f, 0.5);
		particle.m_Size = Smooth(5.0, 0.0, lifeRatio);
	}

	BuildVertexBuffer();

	return true;
	
}


void ParticleSystem::Render()
{
	
}
#endif