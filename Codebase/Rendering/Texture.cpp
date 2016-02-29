#include "Texture.h"

#include <iostream>
#include <sstream>
#include "Renderer.h"

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

void Texture::SetTextureParams(unsigned int flags)
{
	if (!textureLoaded)
		LoadFromFile();

	Renderer::GetInstance()->SetTextureFlags(textureId, flags);
}

void Texture::MeasureMemoryUsageAdd(textureHandle textureId)
{
	s_memoryUsage += Renderer::GetInstance()->TextureMemoryUsage(textureId);
}

void Texture::MeasureMemoryUsageSubtract(textureHandle textureId)
{
	s_memoryUsage -= Renderer::GetInstance()->TextureMemoryUsage(textureId);
}

Texture::Texture(const std::string& filepath, size_t index, bool preload) : filePath(filepath)
{
	textureCopyIndex = index;
	m_referenceCount = 1;
	if (preload) LoadFromFile();
}

Texture::~Texture()
{
#ifdef _DEBUG
		MeasureMemoryUsageSubtract(textureId);
#endif
#ifndef ORBIS
		glDeleteTextures(1, &textureId);
		textureId = 0;
#endif
}

#ifndef ORBIS
void Texture::LoadFromFile()
{
	if (textureId)
	{
#ifdef _DEBUG
		MeasureMemoryUsageSubtract(textureId);
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
		//std::cout << "SOIL loading error: '" << SOIL_last_result() << "' (" << filePath << ")";
		throw std::ios_base::failure(message.str());
	}
	textureLoaded = true;
}
#else
void Texture::LoadFromFile()
{
	std::ifstream file(filePath, std::ios::binary);

	if (!file)
		return;

	sce::Gnf::Header header;
	file.read((char*)&header, sizeof(header));

	if (header.m_magicNumber != sce::Gnf::kMagic)
		return; //This wasn't actually a gnf file!!!

	char* rawContents = new char[header.m_contentsSize];
	file.read((char*)rawContents, header.m_contentsSize);

	sce::Gnf::Contents* contentsDesc = (sce::Gnf::Contents*)rawContents;

	sce::Gnm::SizeAlign dataParams = getTexturePixelsSize(contentsDesc, 0);

	void *pixelsAddr = garlicAllocator.allocate(dataParams);
	sce::Gnm::registerResource(nullptr, ownerHandle, pixelsAddr, dataParams.m_size, filePath.c_str(), sce::Gnm::kResourceTypeTextureBaseAddress, 0);

	file.seekg(getTexturePixelsByteOffset(contentsDesc, 0), ios::cur); //fast forward in the file a bit
	file.read((char*)pixelsAddr, dataParams.m_size);

	textureId = *patchTextures(contentsDesc, 0, 1, &pixelsAddr);
	/*tex->width = tex->apiTexture.getWidth();
	tex->height = tex->apiTexture.getHeight();
	tex->bpp = tex->apiTexture.getDepth();*/

	textureId.setResourceMemoryType(sce::Gnm::kResourceMemoryTypeRO);
	file.close();
	delete rawContents;
	textureLoaded = true;
}
#endif

/// <summary>
/// Loads a texture to it's texture unit for use in shaders
/// </summary>
/// <returns></returns>
void Texture::Load(const shaderResourceLocation& textureUnit)
{
	if (!textureLoaded) 
		LoadFromFile();

	Renderer::GetInstance()->SetTexture(textureUnit, textureId);
}