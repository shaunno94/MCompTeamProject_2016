#pragma once

#include "SOIL2/SOIL2.h"
#pragma comment(lib, "SOIL2/soil2.lib")

#include <string>
#include "Helpers/common.h"

#include "TextureGroup.h"


/*
Texture uploading to gpu management class.
This class uses TextureGroup for managing textures withing a set of gpu texture units.
*/
class TextureSetup {
 public:
	friend class TextureGroup;

	~TextureSetup();

	//Loads texture into a texture unit
	GLuint Load();
	inline int GetTextureUnitIndex() { return textureGroup.textureUnitStart - GL_TEXTURE0 + texSlot; }
	inline GLuint GetTextureId() { return textureId; }

 protected:
	TextureSetup(const std::string& filepath, TextureGroup& textureGroup, bool preload = false);

	void LoadFromFile();

	GLuint textureId;
	GLuint texSlot;
	TextureGroup& textureGroup;
	std::string filepath;
};

