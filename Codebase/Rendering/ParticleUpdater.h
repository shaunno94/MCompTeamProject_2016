#include "ParticleData.h"

class ParticleUpdater
{
public:
	ParticleUpdater() { }
	virtual ~ParticleUpdater() { }

	virtual void update(double dt, ParticleData *p) = 0;
};