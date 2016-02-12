#include "Texture.h"

#include "../Dependencies/SOIL2/SOIL2.h"
#include <iostream>
#include <sstream>


std::unordered_map<std::string, std::vector<Texture*>> Texture::s_textureRecords;
int Texture::s_memoryUsage = 0;


Texture* Texture::Make(const std::string& filePath, bool preload)
{
	Texture* newTexture;
	std::unordered_map<std::string, std::vector<Texture*>>::iterator match = s_textureRecords.find(filePath);
	if (match != s_textureRecords.end())
	{
		newTexture = new Texture(filePath, match->second.size(), preload);
		match->second.push_back(newTexture);
	}
	else
	{
		newTexture = new Texture(filePath, 0, preload);
		s_textureRecords.insert(std::pair<std::string, std::vector<Texture*>>(filePath, std::vector<Texture*>())).first->second.push_back(newTexture);
	}
	return newTexture;
}

Texture* Texture::Get(const std::string& filePath, bool preload)
{
	Texture* newTexture;
	std::unordered_map<std::string, std::vector<Texture*>>::iterator match = s_textureRecords.find(filePath);
	if (match != s_textureRecords.end())
	{
		newTexture = match->second.back();
		newTexture->ReserveCopy();
	}
	else
	{
		newTexture = new Texture(filePath, 0, preload);
		s_textureRecords.insert(std::pair<std::string, std::vector<Texture*>>(filePath, std::vector<Texture*>())).first->second.push_back(newTexture);
	}
	return newTexture;
}

//needs testing!
void Texture::Clear()
{
	//if this was the last reference for the texture
	if (!--(m_referenceCount))
	{
		std::unordered_map<std::string, std::vector<Texture*>>::iterator match = s_textureRecords.find(filePath);
		if (match != s_textureRecords.end())
		{
			std::vector<Texture*>& textures = match->second;
			Texture** foundTex = &textures[textureCopyIndex];
			//double check if referenced texture is correct
			if (this == *foundTex)
			{
				size_t tempCopyIndex = textureCopyIndex;
				std::string tempFilePath = filePath;
				delete this;
				*foundTex = nullptr;
				//if this was the latest texture copy in the collection , then shrink the collection
				if (textures.size() - 1 == tempCopyIndex)
				{
					auto it = textures.end();
					while (it != textures.begin())
					{
						it--;
						if (*it == nullptr)
							it = textures.erase(it);
						else
							break;
					}
					if (textures.size() == 0)
						s_textureRecords.erase(tempFilePath);
				}
			}
		}
	}
}


void Texture::ClearAll()
{
	for (auto it = s_textureRecords.begin(); it != s_textureRecords.end() ; ++it)
	{
		auto end2 = it->second.end();
		for (auto it2 = it->second.begin(); it2 != end2; ++it2)
		{
			if (*it2)
				delete *it2;
		}
	}
	s_textureRecords.clear();
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




Texture::Texture(const std::string& filepath, size_t index, bool preload) : filePath(filepath)
{
	textureId = 0;
	textureCopyIndex = index;
	m_referenceCount = 1;
	if (preload) LoadFromFile();
}

Texture::~Texture()
{
	if(textureId)
	{
#ifdef _DEBUG
		MeasureMemoryUsageSubstract(textureId);
#endif
		glDeleteTextures(1, &textureId);
		textureId = 0;
	}
}


void Texture::LoadFromFile()
{
	if (textureId)
	{
#ifdef _DEBUG
		MeasureMemoryUsageSubstract(textureId);
#endif
		glDeleteTextures(1, &textureId);
		textureId = 0;
	}
	textureId = SOIL_load_OGL_texture(
	              filePath.c_str(),
	              SOIL_LOAD_AUTO,
	              SOIL_CREATE_NEW_ID,
	              SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y
	            );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (textureId)
	{
#ifdef _DEBUG
		MeasureMemoryUsageAdd(textureId);
#endif
	}
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
void Texture::Load(unsigned int textureUnit)
{
	glActiveTexture(GL_TEXTURE0 + textureUnit);
	if (!textureId) LoadFromFile();
	glBindTexture(GL_TEXTURE_2D, textureId);
}