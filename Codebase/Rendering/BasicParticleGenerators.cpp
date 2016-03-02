#include "BasicParticleGenerators.h"
#include "Helpers\RNG.h"

namespace particles
{
	namespace generators
	{
		void BoxPosGen::generate(double dt, ParticleData *p, size_t startId, size_t endId)
		{
			Vec4Graphics posMin{ m_pos.x - m_maxStartPosOffset.x, m_pos.y - m_maxStartPosOffset.y, m_pos.z - m_maxStartPosOffset.z, 1.0 };
			Vec4Graphics posMax{ m_pos.x + m_maxStartPosOffset.x, m_pos.y + m_maxStartPosOffset.y, m_pos.z + m_maxStartPosOffset.z, 1.0 };

			for (size_t i = startId; i < endId; ++i)
			{
				
				p->m_pos[i] = Vec4Graphics(RNG32::Rand(posMin.x, posMax.x), RNG32::Rand(posMin.y, posMax.y),
					RNG32::Rand(posMin.z, posMax.z), RNG32::Rand(posMin.w, posMax.w));
			}
		}

		void RoundPosGen::generate(double dt, ParticleData *p, size_t startId, size_t endId)
		{
			for (size_t i = startId; i < endId; ++i)
			{
				double ang = RNG32::Rand(0.0f,(float) std::_Pi*2.0);
				p->m_pos[i] = m_center + Vec4Graphics(m_radX*sin(ang), m_radY*cos(ang), 0.0, 1.0);
			}
		}

		void BasicColorGen::generate(double dt, ParticleData *p, size_t startId, size_t endId)
		{
			for (size_t i = startId; i < endId; ++i)
			{
				p->m_startCol[i] = Vec4Graphics(RNG32::Rand(m_minStartCol.x, m_maxStartCol.x), RNG32::Rand(m_minStartCol.y, m_maxStartCol.y),
					RNG32::Rand(m_minStartCol.z, m_maxStartCol.z), RNG32::Rand(m_minStartCol.w, m_maxStartCol.w));
				p->m_endCol[i] = Vec4Graphics(RNG32::Rand(m_minEndCol.x, m_maxEndCol.x), RNG32::Rand(m_minEndCol.y, m_maxEndCol.y),
					RNG32::Rand(m_minEndCol.z, m_maxEndCol.z), RNG32::Rand(m_minEndCol.w, m_maxEndCol.w));
			}
		}

		void BasicVelGen::generate(double dt, ParticleData *p, size_t startId, size_t endId)
		{
			for (size_t i = startId; i < endId; ++i)
			{
				p->m_vel[i] = Vec4Graphics(RNG32::Rand(m_minStartVel.x, m_maxStartVel.x), RNG32::Rand(m_minStartVel.y, m_maxStartVel.y), 
					RNG32::Rand(m_minStartVel.z, m_maxStartVel.z), RNG32::Rand(m_minStartVel.w, m_maxStartVel.w));
			}
		}

		void SphereVelGen::generate(double dt, ParticleData *p, size_t startId, size_t endId)
		{
			float phi, theta, v, r;
			for (size_t i = startId; i < endId; ++i)
			{
				phi = RNG32::Rand((float)-std::_Pi, (float)std::_Pi);
				theta = RNG32::Rand((float)-std::_Pi, (float)std::_Pi);
				v = RNG32::Rand(m_minVel, m_maxVel);

				r = v*sinf(phi);
				p->m_vel[i].z = v*cosf(phi);
				p->m_vel[i].x = r*cosf(theta);
				p->m_vel[i].y = r*sinf(theta);
			}
		}

		void BasicTimeGen::generate(double dt, ParticleData *p, size_t startId, size_t endId)
		{
			for (size_t i = startId; i < endId; ++i)
			{
				p->m_time[i].x = p->m_time[i].y = RNG32::Rand(m_minTime, m_maxTime);
				p->m_time[i].z = (float)0.0;
				p->m_time[i].w = (float)1.0 / p->m_time[i].x;
			}
		}
	}
}