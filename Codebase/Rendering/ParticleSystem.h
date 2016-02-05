#pragma once
#include "Math/nclglMath.h"
#include "Mesh.h"
#include "Camera.h"
#include "Particle.h"


enum SystemType {BILLBOARD, TRAIL, BEAM};


class ParticleSystem
{
public:

	ParticleSystem();
	~ParticleSystem();

	struct Vertex
	{
		Vertex()
			:m_Pos(Vec3Graphics(0, 0, 0))
			,m_Colour(Vec4Graphics(0, 0, 0, 0))
			,m_Tex(Vec2Graphics(0, 0))
		{}

		Vec3Graphics m_Pos;
		Vec4Graphics m_Colour;
		Vec2Graphics m_Tex;
	};

	void Update(float delta);
	void Render();

	bool LoadTexture(const std::string& fileName);

protected:
	void RandomizePos();
private:
	Camera*					m_Camera;
	GLuint					m_Texture;
	SystemType				m_SystemType;
	Particle* 				m_Particles;
	unsigned int			m_NumAlive;
	std::vector<Particle>	m_Particles;
	std::vector<Vertex>		m_VertexBuffer;

	Vec3Graphics			m_Force;

};