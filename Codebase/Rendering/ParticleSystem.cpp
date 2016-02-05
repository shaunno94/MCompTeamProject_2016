#include "ParticleSystem.h"

ParticleSystem::ParticleSystem()
	:m_Camera(NULL)
	, m_Texture(0)
	, m_Force(0,-9.81f,0)
	, m_NumAlive(0)
{

}

ParticleSystem::~ParticleSystem()
{
	if (m_Texture != 0)
	{
		glDeleteTextures(1, &m_Texture);
		m_Texture = 0;
	}
}

bool ParticleSystem::LoadTexture(const std::string& fileName)
{
	if (m_Texture != 0)
	{
		glDeleteTextures(1, &m_Texture);
	}

	m_Texture = SOIL_load_OGL_texture(fileName.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	return (m_Texture != 0);
}