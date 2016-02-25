#pragma once

#include "BaseShader.h"
#include "Math/nclglMath.h"
#include "Texture.h"
#include <functional>

class Material
{
	friend class DebugDraw;
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
	static int setUniformValue(std::vector<std::pair<int, T>>& container, BaseShader* shader, const std::string& uniformName, const T& value)
	{
		int location = shader->GetResourceByName(uniformName);
		setUniformValue(container, location, value);
		return location;
	}

	template<typename T>
	static void UpdateUniformValue(std::vector<std::pair<int, T>>& container)
	{
		for (auto it = container.begin(); it != container.end(); ++it)
			UpdateUniform(it->first, it->second);
	}

	static const unsigned int TEXTURE_UNIT_START = 0;
	static Material* s_LastMaterialInUse;

	BaseShader* shader;
	std::vector<std::pair<int, Texture*>> m_uniformTextures;
	bool repeating = true;

public:
	bool hasTranslucency;

	Material(BaseShader* shader, bool hasTranslucency = false);
	virtual ~Material();

	inline BaseShader* GetShader() const
	{
		return shader;
	}

	virtual void Setup();

	int Set(const std::string& uniformName, Texture* texture);

	void Set(int uniformLocation, Texture* texture);

private:
	static void UpdateUniform(int location, const Mat4Graphics& mat4);
	static void UpdateUniform(int location, const Mat3Graphics& mat3);
	static void UpdateUniform(int location, const Vec4Graphics& vec4);
	static void UpdateUniform(int location, const Vec3Graphics& vec3);
	static void UpdateUniform(int location, const Vec2Graphics& vec2);
	static void UpdateUniform(int location, float f);
	static void UpdateUniform(int location, double d);
	static void UpdateUniform(int location, int i);
	static void UpdateUniform(int location, unsigned int u);
};