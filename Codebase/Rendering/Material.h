#pragma once

#include "Shader.h"
#include "Math/nclglMath.h"
#include <functional>
#include <vector>
#include "constants.h"
#include <string>
#include "Texture.h"

class Renderer;

class Material
{
protected:
	template<typename T>
	static void setUniformValue(std::vector<std::pair<int, T>>& container, int uniformLocation, const T& value)
	{
		if (uniformLocation >= 0)
		{
			for (auto it = container.begin(); it != container.end(); ++it)
			{
				if (it->first == uniformLocation)
				{
					it->second = value;
					return;
				}
			}
			container.push_back(std::pair<int, T>(uniformLocation, value));
		}
	}

	template<typename T>
	static int setUniformValue(std::vector<std::pair<int, T>>& container, Shader* shader, const std::string& uniformName, const T& value)
	{
		int location = glGetUniformLocation(shader->GetProgram(), uniformName.c_str());
		setUniformValue(container, location, value);
		return location;
	}

	static const GLuint TEXTURE_UNIT_START = GL_TEXTURE0;


	Shader* shader;
	std::vector<std::pair<int, Texture*>> m_uniformTextures;
	std::vector<std::pair<int, Mat4Graphics>> m_uniformMat4s;

public:
	bool hasTranslucency;

	Material(Shader* shader, bool hasTranslucency = false);
	virtual ~Material();

	inline const Shader* GetShader() const
	{
		return shader;
	}

	int Set(const std::string& uniformName, Texture* texture);
	int Set(const std::string& uniformName, const Mat4Graphics& mat4);

	void Set(int uniformLocation, Texture* texture);
	void Set(int uniformLocation, const Mat4Graphics& mat4);

	virtual void Setup(Renderer* r);
};

