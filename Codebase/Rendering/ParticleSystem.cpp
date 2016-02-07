#include "ParticleSystem.h"

ParticleSystem::ParticleSystem()
	:m_Camera(NULL)
	, m_Texture(0)
	, m_Force(0,-9.81f,0)
	, m_NumAlive(0)
{

}

ParticleSystem::~ParticleSystem()
{
	if (m_Texture != 0)
	{
		glDeleteTextures(1, &m_Texture);
		m_Texture = 0;
	}
}

bool ParticleSystem::LoadTexture(const std::string& fileName)
{
	if (m_Texture != 0)
	{
		glDeleteTextures(1, &m_Texture);
	}

//	m_Texture = SOIL_load_OGL_texture(fileName.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	return (m_Texture != 0);
}

void ParticleSystem::EmitParticle(Particle& particle)
{
	if (m_ParticleEmitter)
		m_ParticleEmitter->EmitParticle(particle);
}

void ParticleSystem::EmitParticles()
{
	for (unsigned int i = 0; i < m_Particles.size(); ++i)
	{
		EmitParticle(m_Particles[i]);
	}
}

void ParticleSystem::BuildVertexBuffer()
{
	Vec3Graphics x = Vec3Graphics(0.5, 0.0, 0.0);
	Vec3Graphics y = Vec3Graphics(0.0, 0.5, 0.0);
	Vec3Graphics z = Vec3Graphics(0.0, 0.0, 1.0);

	Mat4Graphics viewMatrix = m_Camera->BuildViewMatrix();

	m_VertexBuffer.resize(m_Particles.size() * 4, Vertex());

	for (unsigned int i = 0; i < m_Particles.size(); ++i)
	{
		Particle& particle = m_Particles[i];

		unsigned int vertexIndex = i * 4;
		Vertex& v0 = m_VertexBuffer[vertexIndex + 0];
		Vertex& v1 = m_VertexBuffer[vertexIndex + 1];
		Vertex& v2 = m_VertexBuffer[vertexIndex + 2];
		Vertex& v3 = m_VertexBuffer[vertexIndex + 3];

		Vec3Graphics particlePos = viewMatrix * particle.m_Position;

		v0.m_Pos = particlePos + (-x - y) * particle.m_Size;
		v0.m_Tex = Vec2Graphics(0, 1);
		v0.m_Colour = particle.m_Colour;

		v1.m_Pos = particlePos + (x - y) * particle.m_Size;
		v1.m_Tex = Vec2Graphics(1, 1);
		v1.m_Colour = particle.m_Colour;

		v2.m_Pos = particlePos + (x + y) * particle.m_Size;
		v2.m_Tex = Vec2Graphics(1, 0);
		v2.m_Colour = particle.m_Colour;

		v3.m_Pos = particlePos + (-x + y) * particle.m_Size;
		v3.m_Tex = Vec2Graphics(0, 0);
		v3.m_Colour = particle.m_Colour;
	}
}

bool ParticleSystem::Update(float delta)
{
	for (unsigned int i = 0; i < m_Particles.size(); ++i)
	{
		Particle& particle = m_Particles[i];

		particle.m_Age += delta;
		if (particle.m_Age > particle.m_Life)
			EmitParticle(particle);

		particle.m_Velocity += (m_Force * delta);
		particle.m_Position += (particle.m_Velocity * delta);
		particle.m_Colour = Vec4Graphics(255, 255, 255, 0);
		particle.m_Size = 5.0f;
	}

	BuildVertexBuffer();

	return true;
	
}

void ParticleSystem::Render()
{

}