#include "Texture.h"

#include "../Dependencies/SOIL2/SOIL2.h"
#include <iostream>
#include <sstream>


std::unordered_map<std::string, Texture*> Texture::s_textureRecords;
Texture*	Texture::s_boundTextureSlots[MAX_BOUND_TEXTURE_COUNT] = {};
bool		Texture::s_textureSlotActive[MAX_BOUND_TEXTURE_COUNT] = {};
unsigned int Texture::s_nextTextureSlot = 0;

int Texture::s_memoryUsage = 0;



Texture* Texture::Make(const std::string& filePath, bool preload)
{
	std::unordered_map<std::string, Texture*>::const_iterator match = s_textureRecords.find(filePath);
	if (match != s_textureRecords.end())
	{
		++(match->second->m_referenceCount);
		return match->second;
	}

	Texture* newTexture = new Texture(filePath, preload);
	s_textureRecords.insert(std::unordered_map<std::string, Texture*>::value_type(filePath, newTexture));
	return newTexture;
}

void Texture::Clear(Texture* tex)
{
	if (!--(tex->m_referenceCount))
	{
		if (s_boundTextureSlots[tex->texSlot] == tex)
		{
			s_boundTextureSlots[tex->texSlot] = nullptr;
			s_textureSlotActive[tex->texSlot] = false;
		}
		s_textureRecords.erase(tex->filePath);
		delete tex;
	}
}

void Texture::ClearAll()
{
	memset(s_boundTextureSlots, 0, sizeof(Texture*) * MAX_BOUND_TEXTURE_COUNT);
	memset(s_textureSlotActive, 0, sizeof(bool) * MAX_BOUND_TEXTURE_COUNT);
	for (auto i = s_textureRecords.begin(); i != s_textureRecords.end() ; ++i)
	{
		delete i->second;
	}
	s_textureRecords.clear();
}

unsigned int Texture::GetNextTextureSlot()
{
	unsigned int prev;
	unsigned int counter = 0;
	do
	{
		prev = s_nextTextureSlot;
		s_nextTextureSlot = (s_nextTextureSlot + 1) % MAX_BOUND_TEXTURE_COUNT;
		if (++counter == MAX_BOUND_TEXTURE_COUNT) break;
	}
	while (s_textureSlotActive[prev]);
	return prev;
}

void Texture::DrawBoundingReset()
{
	memset(s_textureSlotActive, 0, sizeof(bool) * MAX_BOUND_TEXTURE_COUNT);
}


void Texture::MeasureMemoryUsageAdd(GLuint textureId)
{
	int width;
	int height;
	int r, g, b, a;

	glBindTexture(GL_TEXTURE_2D, textureId);

	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_RED_SIZE, &r);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_GREEN_SIZE, &g);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_BLUE_SIZE, &b);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_ALPHA_SIZE, &a);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);

	glBindTexture(GL_TEXTURE_2D, 0);

	s_memoryUsage += (width * height * ((r + g + b + a) / 8.0f));
}

void Texture::MeasureMemoryUsageSubstract(GLuint textureId)
{
	int width;
	int height;
	int r, g, b, a;

	glBindTexture(GL_TEXTURE_2D, textureId);

	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_RED_SIZE, &r);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_GREEN_SIZE, &g);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_BLUE_SIZE, &b);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_ALPHA_SIZE, &a);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);

	glBindTexture(GL_TEXTURE_2D, 0);

	s_memoryUsage -= (width * height * ((r + g + b + a) / 8.0f));
}




Texture::Texture(const std::string& filepath, bool preload) : filePath(filepath), textureId(0)
{
	texSlot = GetNextTextureSlot();
	m_referenceCount = 1;
	if(preload) Load();
}

Texture::~Texture()
{
	Texture** record = &s_boundTextureSlots[texSlot];

	if(*record == this)
	{
		s_textureSlotActive[TEXTURE_UNIT_START + texSlot] = false;
		glActiveTexture(TEXTURE_UNIT_START + texSlot);
		glBindTexture(GL_TEXTURE_2D, 0);
		delete *record;
		*record = nullptr;
	}
	if(textureId)
	{
		MeasureMemoryUsageSubstract(textureId);
		glDeleteTextures(1, &textureId);
		textureId = 0;
	}
}


void Texture::LoadFromFile()
{
	textureId = SOIL_load_OGL_texture(
	              filePath.c_str(),
	              SOIL_LOAD_AUTO,
	              SOIL_CREATE_NEW_ID,
	              SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y
	            );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if(textureId)
		MeasureMemoryUsageAdd(textureId);
	else
	{
		std::stringstream message;
		message << "SOIL loading error: '" << SOIL_last_result() << "' (" << filePath << ")";
		throw std::ios_base::failure(message.str());
	}
}

/// <summary>
/// Loads a texture to it's texture unit for use in shaders
/// </summary>
/// <returns></returns>
GLuint Texture::Load()
{
	Texture** record = &s_boundTextureSlots[texSlot];
	if (*record != nullptr)
	{
		if (*record != this)
		{
			if (s_textureSlotActive[texSlot])
			{
				texSlot = GetNextTextureSlot();
				record = &s_boundTextureSlots[texSlot];
			}
			*record = this;
			glActiveTexture(TEXTURE_UNIT_START + texSlot);
			if (!textureId) LoadFromFile();
			glBindTexture(GL_TEXTURE_2D, textureId);
		}
	}
	else
	{
		*record = this;
		glActiveTexture(TEXTURE_UNIT_START + texSlot);
		if (!textureId) LoadFromFile();
		glBindTexture(GL_TEXTURE_2D, textureId);
	}
	s_textureSlotActive[texSlot] = true;

	return textureId;
}