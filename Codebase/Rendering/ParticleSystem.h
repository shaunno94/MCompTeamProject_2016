#pragma once
#include "Math/nclglMath.h"
#include "Particle.h"
#include "ParticleEmitter.h"
#include "Material.h"
#include "Helpers\interpolation.h"

class ParticleSystem
{
public:
	ParticleSystem(Material* material, GameObject* attatchment, Scene* scene, unsigned int maxParticles);
	~ParticleSystem();
	void Update(float delta);

protected:
	void EmitParticle(Particle* particle, float dt);

private:
	GameObject*				m_Attatchment;
	Scene*					m_Scene;
	std::vector<Particle*>	m_Particles;
	unsigned int			m_NumParticles;
	Material*				m_Material;

	const float MIN_FORCE_XZ = -20.0f;
	const float MAX_FORCE_XZ = 20.0f;
	const Vec3Graphics m_Scale = Vec3Graphics(0.08, 0.08, 0.08);
	const Vec3Graphics m_Offset = Vector3Simple(0.0, 3.0, 10.0);
};