#pragma once

#include "GL/glew.h"
#include "Shader.h"
#include "Math/nclglMath.h"
#include <functional>
#include <vector>
#include "constants.h"
#include <string>
#include "TextureSetup.h"
#include <utility>

class SceneObject;
class Renderer;
class Camera;

class Material {
 protected:
	Shader* shader;
	bool hasTranslucency;
	bool castsShadows;
	Vec4Graphics colour;
	std::vector<std::pair<int, TextureSetup*>> uniformTextures;

public:

	Material(Shader* shader, bool hasTranslucency);
	virtual ~Material();

	inline bool HasTranslucency() const { return hasTranslucency; }

	inline const Shader* GetShader() const { return shader; }

	void AddTexture(const std::string& uniformName, TextureSetup* texture);

	void UpdateTextures();
};

