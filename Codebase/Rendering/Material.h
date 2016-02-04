#pragma once

#include "GL/glew.h"
#include "Shader.h"
#include "Math/nclglMath.h"
#include <functional>
#include <vector>
#include "constants.h"
#include <string>
#include "Texture.h"
#include <utility>

class SceneObject;
class Renderer;
class Camera;

class Material {
 protected:
	Shader* shader;
	Vec4Graphics colour;
	std::vector<std::pair<int, Texture*>> uniformTextures;

public:
	bool hasTranslucency;

	Material(Shader* shader, bool hasTranslucency = false);
	virtual ~Material();

	inline const Shader* GetShader() const { return shader; }

	void AddTexture(const std::string& uniformName, Texture* texture);

	void UpdateTextures();
};

