#include "ParticleSystem.h"
#include "CubeEmitter.h"
#include "./Helpers/RNG.h"

CubeEmitter::CubeEmitter()
: MinWidth(-1)
, MaxWidth(1)
, MinHeight(-1)
, MaxHeight(1)
, MinDepth(-1)
, MaxDepth(1)
, MinSpeed(10)
, MaxSpeed(20)
, MinLifetime(3)
, MaxLifetime(5)
, Origin(Vec3Graphics(0,0,0))
{}

void CubeEmitter::EmitParticle(Particle& particle)
{	
	float x = RNG32::Rand(MinWidth, MaxWidth);
	float y = RNG32::Rand(MinHeight, MaxHeight);
	float z = RNG32::Rand(MinDepth, MaxDepth);

	float lifetime = RNG32::Rand(MinLifetime, MaxLifetime);
	float speed = RNG32::Rand(MinSpeed, MaxSpeed);

	Vec3Graphics vector(x, y, z);

	particle.m_Position = vector + Origin;
	particle.m_Velocity = vector.Normalize() * speed;

	particle.m_Life = lifetime;
	particle.m_Age = 0;
}

void CubeEmitter::DebugRender()
{

}
