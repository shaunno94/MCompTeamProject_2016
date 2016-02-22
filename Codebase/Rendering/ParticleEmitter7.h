#pragma once
#include <vector>
#include "ParticleGenerator.h"

class ParticleEmitter7
{
public:
	ParticleEmitter7() { }
	virtual ~ParticleEmitter7() { }

	// calls all the generators and at the end it activates (wakes) particle
	virtual void emit(double dt, ParticleData* p);

	void addGenerator(ParticleGenerator* gen)
	{
		m_generators.push_back(gen);
	}

protected:
	std::vector<ParticleGenerator*> m_generators;
public:
	float m_emitRate{ 0.0 };
};