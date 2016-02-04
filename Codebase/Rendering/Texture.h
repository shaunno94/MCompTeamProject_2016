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
	GLuint Load();
	inline int GetTextureUnitIndex()
	{
		return TEXTURE_UNIT_START - GL_TEXTURE0 + texSlot;
	}
	inline GLuint GetTextureId()
	{
		return textureId;
	}

protected:
	static const GLuint TEXTURE_UNIT_START = GL_TEXTURE0;
	static const unsigned int MAX_BOUND_TEXTURE_COUNT = 32;

	static std::unordered_map<std::string, Texture*> s_textureRecords;

	static Texture* s_boundTextureSlots[MAX_BOUND_TEXTURE_COUNT];
	static bool s_textureSlotActive[MAX_BOUND_TEXTURE_COUNT];
	static unsigned int s_nextTextureSlot;

	static Texture* Make(const std::string& filePath, bool preload = false);
	static void Clear(Texture* tex);
	static void ClearAll();

	static unsigned int GetNextTextureSlot();
	static void DrawBoundingReset();

	static int s_memoryUsage;
	void MeasureMemoryUsageAdd(GLuint textureId);
	void MeasureMemoryUsageSubstract(GLuint textureId);


	Texture(const std::string& filepath, bool preload = false);

	void LoadFromFile();

	GLuint textureId;
	GLuint texSlot;
	std::string filePath;
	unsigned int m_referenceCount;
};

