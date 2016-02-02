#include "TextureSetup.h"

#include "../Dependencies/SOIL2/SOIL2.h"
#include <iostream>
#include "TextureGroup.h"
#include <sstream>


TextureSetup::TextureSetup(const std::string& filepath, TextureGroup& textureGroup, bool preload) : textureGroup(textureGroup), filepath(filepath), textureId(0) {
	texSlot = textureGroup.GetNextTextureSlot();
	if(preload) Load();
}

TextureSetup::~TextureSetup() {
	TextureSetup** record = textureGroup.GetTextureRecord(texSlot);
	if(*record != nullptr) {
		if(*record == this) {
			glActiveTexture(textureGroup.textureUnitStart + texSlot);
			glBindTexture(GL_TEXTURE_2D, 0);
			delete *record;
			*record = nullptr;
		}
	}
	if(textureId) {
		textureGroup.MeasureMemoryUsageSubstract(textureId);
		glDeleteTextures(1, &textureId);
	}
}


void TextureSetup::LoadFromFile() {
	textureId = SOIL_load_OGL_texture(
	              filepath.c_str(),
	              SOIL_LOAD_AUTO,
	              SOIL_CREATE_NEW_ID,
	              SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y
	            );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if(textureId)
		textureGroup.MeasureMemoryUsageAdd(textureId);
	else {
		std::stringstream message;
		message << "SOIL loading error: '" << SOIL_last_result() << "' (" << filepath << ")";
		throw std::ios_base::failure(message.str());
	}
}

/*
Loads a texture to it's texture unit for use in shaders
*/
GLuint TextureSetup::Load() {
	TextureSetup** record = textureGroup.GetTextureRecord(texSlot);
	if(*record != NULL) {
		if(*record != this) {
			texSlot = textureGroup.GetNextTextureSlot();
			record = textureGroup.GetTextureRecord(texSlot);
			*record = this;
		}
		glActiveTexture(textureGroup.textureUnitStart + texSlot);
		if(!textureId) LoadFromFile();
		glBindTexture(GL_TEXTURE_2D, textureId);
	} else {
		*record = this;
		glActiveTexture(textureGroup.textureUnitStart + texSlot);
		if(!textureId) LoadFromFile();
		glBindTexture(GL_TEXTURE_2D, textureId);
	}
	return textureId;
}