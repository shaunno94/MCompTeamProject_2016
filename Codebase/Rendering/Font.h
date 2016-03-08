#pragma once

class Font {

public:

	Font(Texture* tex, unsigned int xCount, unsigned int yCount) {
		this->texture = tex;
		this->xCount = xCount;
		this->yCount = yCount;
	}
	~Font() {}

	Texture* getTexture() { return texture; }
	int getXCount() { return xCount; }
	int getYCount() { return yCount; }

protected:
	Texture* texture;
	int		xCount;
	int		yCount;
};