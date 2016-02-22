#include "BasicParticleGenerator.h"
#include "Helpers\RNG.h"

#include <assert.h>
#include <algorithm>
#include <math.h>

void BoxPosGen::generate(double dt, ParticleData* p, int startId, int endId)
{
	Vec4Graphics posMin{ m_pos.x - m_maxStartPosOffset.x, m_pos.y - m_maxStartPosOffset.y, m_pos.z - m_maxStartPosOffset.z, 1.0 };
	Vec4Graphics posMax{ m_pos.x + m_maxStartPosOffset.x, m_pos.y + m_maxStartPosOffset.y, m_pos.z + m_maxStartPosOffset.z, 1.0 };

	for (size_t i = startId; i < endId; ++i)
	{
		p->m_pos[i] = RandVec4(posMin, posMax);
	}
}

void RoundPosGen::generate(double dt, ParticleData* p, int startId, int endId)
{
	for (size_t i = startId; i < endId; ++i)
	{
		double ang = RNG32::Rand(0.0f, (float) std::_Pi*2.0);
		p->m_pos[i] = m_center + Vec4Graphics(m_radX*sin(ang), m_radY*cos(ang), 0.0, 1.0);
	}
}

void BasicColorGen::generate(double dt, ParticleData* p, int startId, int endId)
{
	for (size_t i = startId; i < endId; ++i)
	{
		p->m_startCol[i] = RandVec4(m_minStartCol, m_maxStartCol);
		p->m_endCol[i] = RandVec4(m_minEndCol, m_maxEndCol);
	}
}

void BasicVelGen::generate(double dt, ParticleData* p, int startId, int endId)
{
	for (size_t i = startId; i < endId; ++i)
	{
		p->m_vel[i] = RandVec4(m_minStartVel, m_maxStartVel);
	}
}

void SphereVelGen::generate(double dt, ParticleData* p, int startId, int endId)
{
	float phi, theta, v, r;
	for (size_t i = startId; i < endId; ++i)
	{
		phi = RNG32::Rand((float)std::_Pi, (float)std::_Pi);
		theta = RNG32::Rand((float)std::_Pi, (float)std::_Pi);
		v = RNG32::Rand(m_minVel, m_maxVel);

		r = v*sinf(phi);
		p->m_vel[i].z = v*cosf(phi);
		p->m_vel[i].x = r*cosf(theta);
		p->m_vel[i].y = r*sinf(theta);
	}
}

void BasicTimeGen::generate(double dt, ParticleData* p, int startId, int endId)
{
	for (size_t i = startId; i < endId; ++i)
	{
		p->m_time[i].x = p->m_time[i].y = RNG32::Rand(m_minTime, m_maxTime);
		p->m_time[i].z = (float)0.0;
		p->m_time[i].w = (float)1.0 / p->m_time[i].x;
	}
}
