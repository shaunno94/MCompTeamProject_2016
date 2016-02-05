#pragma once

#include <vector>
#include "GL/glew.h"
#include <bitset>
#include <map>
#include <string>

class TextureSetup;

/*
Helper for tracking gpu texture slot usage.
New textures are assigned a slot in a circular fashion.
The texture group size needs to be at least the number of textures required at the same time to render a frame.
*/
class TextureGroup {
 public:
	static const unsigned int TEXTURE_GROUP_SIZE = 32;

	TextureGroup(unsigned int textureUnitStart = GL_TEXTURE0);
	~TextureGroup();

	//gets next managed gpu texture slot in a circular fashion.
	inline GLuint GetNextTextureSlot() {
		GLuint prev;
		unsigned int counter = 0;
		do {
			if(++counter == TEXTURE_GROUP_SIZE) break;
			prev = currentTextureSlot;
			currentTextureSlot = (currentTextureSlot + 1) % TEXTURE_GROUP_SIZE;
		} while(recordLogs.at(prev));
		recordLogs.set(prev);
		return prev;
	}


	inline void NewFrame() { recordLogs.reset(); }

	inline TextureSetup** GetTextureRecord(unsigned int slot) {
		recordLogs.set(slot);
		return &m_activeTextureSlots[slot];
	}

	const GLuint textureUnitStart;

	void MeasureMemoryUsageAdd(GLuint textureId);
	void MeasureMemoryUsageSubstract(GLuint textureId);

	inline int GetMemoryUsage() { return memoryUsage; }

	TextureSetup* GetTexture(const std::string& filePath, bool preload = false);

 protected:
	typedef std::map<std::string, TextureSetup*> TextureRecordMap;
	TextureRecordMap m_textureRecords;
	std::vector<TextureSetup*> m_activeTextureSlots;

	GLuint currentTextureSlot;
	std::bitset<TEXTURE_GROUP_SIZE> recordLogs;

	int memoryUsage;
};

