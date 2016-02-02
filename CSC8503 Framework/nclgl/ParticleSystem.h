#include "Math/nclglMath.h"
#include "nclgl\Mesh.h"

struct Particle
{
	Vec3Physics position;
	Vec3Physics oldPos;
	Vec3Physics velocity;
	Vec3Graphics colour;

	float life;
	float size;
};

class ParticleSystem
{
public:
protected:
private:
	GLuint* texture;
	int systemType;
	Particle * particles;
	int numAlive;
	
};