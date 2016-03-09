#pragma once
#include "Math/nclglMath.h"
#include "Particle.h"
#include "ParticleEmitter.h"
#include "Material.h"
#include "Helpers\interpolation.h"

class ParticleSystem
{
public:

	ParticleSystem(ParticleEmitter* emitter, Material* material, Texture* texture, unsigned int numParticles);
	~ParticleSystem();

	void EmitParticles();
	void BuildVertexBuffer();
	bool Update(float delta);
	void Render();

protected:
	void RandomizePos();
	void EmitParticle(Particle& particle);
private:

	GameObject*				m_Object;
	ParticleEmitter*		m_ParticleEmitter;
	Texture*				m_Texture;
	unsigned int			m_NumAlive;
	Particle*				m_Particles;
	unsigned int			m_NumParticles;

	Vec3Graphics*			m_Centre;

	Vec3Graphics			m_Force;
	Material*				m_Material;
	Texture*				m_texture;
};