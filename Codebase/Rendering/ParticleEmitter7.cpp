#include "ParticleEmitter7.h"
#include <algorithm>

void ParticleEmitter7::emit(double dt, ParticleData *p)
{
	const int maxNewParticles = static_cast<size_t>(dt*m_emitRate);
	const int startId = p->getAliveCount();
	const int endId = std::min(startId + maxNewParticles, p->getCount() - 1);

	for (ParticleGenerator* &gen : m_generators)
		gen->generate(dt, p, startId, endId);

	for (size_t i = startId; i < endId; ++i)
	{
		p->wake(i);
	}
}