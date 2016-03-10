#pragma once
#include "Math/nclglMath.h"
#include "Particle.h"
#include "ParticleEmitter.h"
#include "Material.h"
#include "Helpers\interpolation.h"

class ParticleSystem
{
public:

	ParticleSystem(ParticleEmitter* emitter, Material* material, GameObject* attatchment, Scene* scene, unsigned int maxParticles);
	~ParticleSystem();
	void Update(float delta);

protected:
	void EmitParticle(Particle* particle, float dt);

private:

	GameObject*				m_Attatchment;
	Scene*					m_Scene;
	ParticleEmitter*		m_ParticleEmitter;
	unsigned int			m_NumAlive;
	std::vector<Particle*>	m_Particles;
	unsigned int			m_NumParticles;

	std::vector<Vec3Graphics> m_Centre;

	Vec3Graphics			m_Force;
	Material*				m_Material;
	Texture*				m_texture;

	btTransform transform;

	const float MIN_FORCE_XZ = -0.1f;
	const float MAX_FORCE_XZ = 0.1f;
	const float MIN_FORCE_Y = 0.1f;
	const float MAX_FORCE_Y = 0.2f;
};