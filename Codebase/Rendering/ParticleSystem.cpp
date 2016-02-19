#include "ParticleSystem.h"

ParticleSystem::ParticleSystem(ParticleEmitter* emitter, const std::string& texturePath,
                                unsigned int numParticles, Camera* camera, Renderer* renderer)
	:m_Camera(camera)
	, m_Texture(NULL)
	, m_Force(0,-9.81f,0)
	, m_NumAlive(0)
	, m_ParticleEmitter(emitter)
	, m_NumParticles(numParticles)
	, renderer(renderer)
{
	LoadTexture(texturePath);
	pos[numParticles][3];
	tex[numParticles][2];
	colour[numParticles][4];
}

ParticleSystem::~ParticleSystem()
{

}

bool ParticleSystem::LoadTexture(const std::string& texturePath)
{
	m_Texture = Texture::Get(texturePath);

	return (m_Texture != 0);
}

void ParticleSystem::EmitParticle(Particle& particle)
{
	if (m_ParticleEmitter)
		m_ParticleEmitter->EmitParticle(particle);
	else
	{
		//some form of default ways to emit particles
	}
}

void ParticleSystem::EmitParticles()
{
	for (unsigned int i = 0; i < m_Particles.size(); ++i)
	{
		EmitParticle(m_Particles[i]);
	}
}

void ParticleSystem::BuildVertexBuffer()
{
	glGenVertexArrays(1, &arrayObject);
	glBindVertexArray(arrayObject);
	glGenBuffers(3, bufferObject);

	Vec3Graphics x = Vec3Graphics(0.5, 0.0, 0.0);
	Vec3Graphics y = Vec3Graphics(0.0, 0.5, 0.0);
	Vec3Graphics z = Vec3Graphics(0.0, 0.0, 1.0);

	if (m_Camera != NULL)
	{
		m_Camera->BuildViewMatrix();
	}


	for (unsigned int i = 0; i < m_Particles.size(); ++i)
	{
		Particle& particle = m_Particles[i];

		unsigned int vertexIndex = i * 4;

		pos[i][0] = (particle.m_Position + ((-x - y) * particle.m_Size)).x;
		pos[i][1] = (particle.m_Position + ((-x - y) * particle.m_Size)).y;
		pos[i][2] = (particle.m_Position + ((-x - y) * particle.m_Size)).z;

		colour[i][0] = (particle.m_Colour).x;
		colour[i][1] = (particle.m_Colour).y;
		colour[i][2] = (particle.m_Colour).z;
		colour[i][3] = (particle.m_Colour).w;
	}

	glBindBuffer(GL_ARRAY_BUFFER, bufferObject[POS_BUFFER]);
	glBufferData(GL_ARRAY_BUFFER, m_NumParticles * sizeof(Vec3Graphics), pos, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, bufferObject[TEX_BUFFER]);
	glBufferData(GL_ARRAY_BUFFER, m_NumParticles * sizeof(Vec2Graphics), tex, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, bufferObject[COLOUR_BUFFER]);
	glBufferData(GL_ARRAY_BUFFER, m_NumParticles * sizeof(Vec4Graphics), colour, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);
}

bool ParticleSystem::Update(float delta)
{
	for (unsigned int i = 0; i < m_Particles.size(); ++i)
	{
		Particle& particle = m_Particles[i];

		particle.m_Age += delta;
		if (particle.m_Age > particle.m_Life)
			EmitParticle(particle);

		float lifeRatio = ClampValues((particle.m_Age / particle.m_Life), 0.0f, 1.0f);
		particle.m_Velocity += (m_Force * delta);
		particle.m_Position += (particle.m_Velocity * delta);
		particle.m_Colour = Vec4Graphics(1.0f, 0.0f, 0.0f, 0.5);
		particle.m_Size = Smooth(5.0, 0.0, lifeRatio);
	}

	BuildVertexBuffer();

	return true;

}


void ParticleSystem::Render()
{
	GLuint textureID = m_Texture->GetTextureId();
	glGenTextures(1, &textureID);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_BUFFER, m_Texture->GetTextureId());

	//glBindAttribLocation(shader->GetProgram(), 0, "pos");
	//glBindAttribLocation(shader->GetProgram(), 1, "tex");
	//glBindAttribLocation(shader->GetProgram(), 2, "colour");

	//glUseProgram(shader->GetProgram());

	glDrawArrays(GL_LINE_LOOP, 0, m_NumParticles);

	
	glUseProgram(0);

}
