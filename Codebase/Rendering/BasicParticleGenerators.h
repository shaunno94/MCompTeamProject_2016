#pragma once

#include <vector>
#include "BasicParticles.h"

namespace particles
{
	namespace generators
	{
		class BoxPosGen : public ParticleGenerator
		{
		public:
			Vec4Graphics m_pos{ 0.0f,0.0f,0.0f,0.0f };
			Vec4Graphics m_maxStartPosOffset{ 0.0f,0.0f,0.0f,0.0f };
		public:
			BoxPosGen() { }

			virtual void generate(double dt, ParticleData *p, size_t startId, size_t endId) override;
		};

		class RoundPosGen : public ParticleGenerator
		{
		public:
			Vec4Graphics m_center{ 0.0f,0.0f,0.0f,0.0f };
			float m_radX{ 0.0 };
			float m_radY{ 0.0 };
		public:
			RoundPosGen() { }
			RoundPosGen(const Vec4Graphics &center, double radX, double radY)
				: m_center(center)
				, m_radX((float)radX)
				, m_radY((float)radY)
			{ }

			virtual void generate(double dt, ParticleData *p, size_t startId, size_t endId) override;
		};

		class BasicColorGen : public ParticleGenerator
		{
		public:
			Vec4Graphics m_minStartCol{ 0.0f, 0.0f, 0.0f, 0.0f };
			Vec4Graphics m_maxStartCol{ 0.0f, 0.0f, 0.0f, 0.0f };
			Vec4Graphics m_minEndCol{ 0.0f, 0.0f, 0.0f, 0.0f };
			Vec4Graphics m_maxEndCol{ 0.0f, 0.0f, 0.0f, 0.0f };
		public:
			BasicColorGen() { }

			virtual void generate(double dt, ParticleData *p, size_t startId, size_t endId) override;
		};

		class BasicVelGen : public ParticleGenerator
		{
		public:
			Vec4Graphics m_minStartVel{ 0.0f, 0.0f, 0.0f, 0.0f };
			Vec4Graphics m_maxStartVel{ 0.0f, 0.0f, 0.0f, 0.0f };
		public:
			BasicVelGen() { }

			virtual void generate(double dt, ParticleData *p, size_t startId, size_t endId) override;
		};

		class SphereVelGen : public ParticleGenerator
		{
		public:
			float m_minVel{ 0.0f };
			float m_maxVel{ 0.0f };
		public:
			SphereVelGen() { }

			virtual void generate(double dt, ParticleData *p, size_t startId, size_t endId) override;
		};

		class BasicTimeGen : public ParticleGenerator
		{
		public:
			float m_minTime{ 0.0 };
			float m_maxTime{ 0.0 };
		public:
			BasicTimeGen() { }

			virtual void generate(double dt, ParticleData *p, size_t startId, size_t endId) override;
		};
	}
}