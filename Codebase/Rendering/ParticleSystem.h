#pragma once
#ifndef ORBIS
#include "Math/nclglMath.h"
#include "Camera.h"
#include "Particle.h"
#include "ParticleEmitter.h"
#include "Material.h"
#include "Helpers\interpolation.h"
#include "Renderer.h"

class ParticleSystem
{
public:

	ParticleSystem(ParticleEmitter* emitter, Material* material, Texture* texture, Mesh* mesh, unsigned int numParticles);
	~ParticleSystem();

	void EmitParticles();
	void BuildVertexBuffer();
	bool Update(float delta);
	void Render();

protected:
	void RandomizePos();
	void EmitParticle(Particle& particle);
private:

	Camera*					m_Camera;
	ParticleEmitter*		m_ParticleEmitter;
	Texture*				m_Texture;
	unsigned int			m_NumAlive;
	std::vector<Particle>	m_Particles;
	unsigned int			m_NumParticles;

	GLfloat**				pos;

	Vec3Graphics			m_Force;
	Material*				m_Material;
	Mesh*					m_Mesh;
	Texture*				m_texture;
};
#endif