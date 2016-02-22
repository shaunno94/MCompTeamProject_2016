#include "PartSystem.h"

PartSystem::PartSystem(int maxCount)
{
	m_count = maxCount;
	m_particles.generate(maxCount);
	m_aliveParticles.generate(maxCount);

	for (int i = 0; i < maxCount; ++i)
		m_particles.m_alive[i] = false;
}

void PartSystem::update(double dt)
{
	for (auto& em : m_emitters)
	{
		em->emit(dt, &m_particles);
	}

	for (size_t i = 0; i < m_count; ++i)
	{
		m_particles.m_acc[i] = Vec4Graphics(0.0f, 0.0f, 0.0f, 0.0f);
	}

	for (auto& up : m_updaters)
	{
		up->update(dt, &m_particles);
	}

	//ParticleData::copyOnlyAlive(&m_particles, &m_aliveParticles);
}

void PartSystem::reset()
{
	m_particles.m_countAlive = 0;
}

//size_t PartSystem::computeMemoryUsage(const PartSystem& p)
//{
//	return 2 * ParticleData::computeMemoryUsage(p.m_particles);
//}