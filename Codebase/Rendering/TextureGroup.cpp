#include "TextureGroup.h"
#include "TextureSetup.h"

TextureGroup::TextureGroup(unsigned int textureUnitStart)
	: textureUnitStart(textureUnitStart),
	  currentTextureSlot(0),
	  m_activeTextureSlots(std::vector<TextureSetup*>(TEXTURE_GROUP_SIZE, NULL)),
	  recordLogs(0x000000),
	  memoryUsage(0) {
}


TextureGroup::~TextureGroup() {
	for(auto it = m_textureRecords.begin(); it != m_textureRecords.end(); it++) {
		delete it->second;
		it->second = nullptr;
	}
}


void TextureGroup::MeasureMemoryUsageAdd(GLuint textureId) {
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

	memoryUsage += (width * height * ((r + g + b + a) / 8.0f));
}

void TextureGroup::MeasureMemoryUsageSubstract(GLuint textureId) {
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

	memoryUsage -= (width * height * ((r + g + b + a) / 8.0f));
}


TextureSetup* TextureGroup::GetTexture(const std::string& filePath, bool preload) {
	TextureRecordMap::iterator it = m_textureRecords.find(filePath);
	if(it != m_textureRecords.end())
		return it->second;

	TextureSetup* newTexture = new TextureSetup(filePath, *this, preload);
	m_textureRecords.insert(TextureRecordMap::value_type(filePath, newTexture));

	return newTexture;
}