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
	static void setUniformValue(std::vector<std::pair<shaderResourceLocation, T>>& container, const shaderResourceLocation& uniformLocation, const T& value)
	{
		if (uniformLocation.inUse())
		{
			for (auto it = container.begin(); it != container.end(); ++it)
			{
				if (it->first.eq(uniformLocation))
				{
					it->second = value;
					return;
				}
			}
			container.push_back(std::pair<shaderResourceLocation, T>(uniformLocation, value));
		}
	}

	template<typename T>
	static shaderResourceLocation setUniformValue(std::vector<std::pair<shaderResourceLocation, T>>& container, BaseShader* shader, const std::string& uniformName, const T& value)
	{
		shaderResourceLocation location = shader->GetResourceByName(uniformName);
		setUniformValue(container, location, value);
		return location;
	}

	template<typename T>
	static void UpdateUniformValue(std::vector<std::pair<shaderResourceLocation, T>>& container)
	{
		for (auto it = container.begin(); it != container.end(); ++it)
			UpdateUniform(it->first, it->second);
	}

	static Material* s_LastMaterialInUse;

	BaseShader* shader;
	std::vector<std::pair<shaderResourceLocation, Texture*>> m_uniformTextures;
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

	shaderResourceLocation Set(const std::string& uniformName, Texture* texture);

	void Set(const shaderResourceLocation& uniformLocation, Texture* texture);

private:
	static void UpdateUniform(const shaderResourceLocation& location, const Mat4Graphics& mat4);
	static void UpdateUniform(const shaderResourceLocation& location, const Mat3Graphics& mat3);
	static void UpdateUniform(const shaderResourceLocation& location, const Vec4Graphics& vec4);
	static void UpdateUniform(const shaderResourceLocation& location, const Vec3Graphics& vec3);
	static void UpdateUniform(const shaderResourceLocation& location, const Vec2Graphics& vec2);
	static void UpdateUniform(const shaderResourceLocation& location, float f);
	static void UpdateUniform(const shaderResourceLocation& location, double d);
	static void UpdateUniform(const shaderResourceLocation& location, int i);
	static void UpdateUniform(const shaderResourceLocation& location, unsigned int u);
};
