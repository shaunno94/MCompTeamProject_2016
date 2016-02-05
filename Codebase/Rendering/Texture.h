#pragma once

#include "GL/glew.h"
#include "SOIL2/SOIL2.h"
#pragma comment(lib, "SOIL2/soil2.lib")

#include <string>
#include <unordered_map>
#include <vector>
#include "Helpers/common.h"


/*
Texture uploading to gpu management class.
*/
class Texture
{
public:
	~Texture();

	//Loads texture into a texture unit
	void Load(unsigned int textureUnit);
	void LoadFromFile();
	inline GLuint GetTextureId()
	{
		if (!textureId) LoadFromFile();
		return textureId;
	}

	static inline int GetMemoryUsage()
	{
		return s_memoryUsage;
	}

	Texture& operator=(const Texture&) = delete;
	Texture(const Texture&) = delete;

protected:

	static std::unordered_map<std::string, std::vector<Texture*>> s_textureRecords;

	static Texture* Make(const std::string& filePath, bool preload = false);
	static Texture* Get(const std::string& filePath, bool preload = false);
	static void Clear(Texture* tex);
	static void ClearAll();

	static int s_memoryUsage;
	void MeasureMemoryUsageAdd(GLuint textureId);
	void MeasureMemoryUsageSubstract(GLuint textureId);


	Texture(const std::string& filepath, size_t textureCopyIndex, bool preload = false);


	GLuint textureId;
	size_t textureCopyIndex;
	std::string filePath;
	unsigned int m_referenceCount;
};

