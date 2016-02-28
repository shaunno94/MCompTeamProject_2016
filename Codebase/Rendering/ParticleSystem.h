#pragma once
#ifndef ORBIS
#include "Math/nclglMath.h"
#include "Camera.h"
#include "Particle.h"
#include "ParticleEmitter.h"
#include "Material.h"
#include "Helpers\interpolation.h"
#include "Renderer.h"

enum ParticleBuffer {
	POS_BUFFER, TEX_BUFFER, COLOUR_BUFFER
};

class ParticleSystem
{
public:

	ParticleSystem(ParticleEmitter* emitter, const std::string& texturePath, unsigned int numParticles, Camera* camera, Renderer* renderer);
	~ParticleSystem();

	void EmitParticles();
	void BuildVertexBuffer();
	bool Update(float delta);
	void Render();

	bool LoadTexture(const std::string& fileName);

protected:
	void RandomizePos();
	void EmitParticle(Particle &particle);
private:

	Camera*					m_Camera;
	ParticleEmitter*		m_ParticleEmitter;
	Texture*				m_Texture;
	unsigned int			m_NumAlive;
	std::vector<Particle>	m_Particles;
	unsigned int			m_NumParticles;

	GLuint					arrayObject;
	GLuint					bufferObject[MAX_BUFFER];
	GLfloat**				pos;
	GLfloat**				tex;
	GLfloat**				colour;

	Vec3Graphics			m_Force;
	Renderer*				renderer;

};
#endif