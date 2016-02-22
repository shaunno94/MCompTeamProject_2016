#pragma once

#include "ParticleData.h"
#include "ParticleEmitter7.h"
#include "ParticleUpdater.h"
#include <vector>

class PartSystem
{

public:
	explicit PartSystem(int maxCount);
	virtual ~PartSystem() { }

	PartSystem(const PartSystem &) = delete;
	PartSystem &operator=(const PartSystem &) = delete;

	virtual void update(double dt);
	virtual void reset();

	virtual int numAllParticles() const { return m_particles.getCount(); }
	virtual int numAliveParticles() const { return m_particles.getAliveCount(); }

	void addEmitter(ParticleEmitter7* em) { m_emitters.push_back(em); }
	void addUpdater(ParticleUpdater* up) { m_updaters.push_back(up); }

	ParticleData *finalData() { return &m_particles; }

	static size_t computeMemoryUsage(const PartSystem &p);

protected:
	ParticleData m_particles;
	ParticleData m_aliveParticles;

	size_t m_count;

	std::vector<ParticleEmitter7*> m_emitters;
	std::vector<ParticleUpdater*> m_updaters;
};