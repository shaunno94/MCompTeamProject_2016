#include "ParticleData.h"
#include "Helpers\RNG.h"

class ParticleGenerator
{
public:
	ParticleGenerator();
	virtual ~ParticleGenerator();

	virtual void generate(double dt, ParticleData* p, int startId, int endId) = 0;
	Vec4Graphics RandVec4(Vec4Graphics& a, Vec4Graphics& b)
	{
		float x = RNG32::Rand(a.x, b.x);
		float y = RNG32::Rand(a.y, b.y);
		float z = RNG32::Rand(a.z, b.z);
		float w = RNG32::Rand(a.w, b.w);

		return Vec4Graphics(x, y, z, w);
	}
};