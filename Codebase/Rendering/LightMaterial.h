#pragma once
#include "Material.h"
enum shadowType { _NONE, _2D, _CUBE };

class LightMaterial : public Material
{
protected:
	std::vector<std::pair<shaderResourceLocation, Mat4Graphics>> m_uniformMat4s;
	std::vector<std::pair<shaderResourceLocation, Vec4Graphics>> m_uniformVec4s;
	std::vector<std::pair<shaderResourceLocation, Vec3Graphics>> m_uniformVec3s;
	std::vector<std::pair<shaderResourceLocation, Vec2Graphics>> m_uniformVec2s;
	std::vector<std::pair<shaderResourceLocation, float>> m_uniformFloats;
	std::vector<std::pair<shaderResourceLocation, int>> m_uniformInts;



public:
	LightMaterial(BaseShader* shader, bool hasTranslucency = false);
	~LightMaterial();

	virtual void Setup() override;

	shaderResourceLocation Set(const std::string& uniformName, const Mat4Graphics& mat4);
	shaderResourceLocation Set(const std::string& uniformName, const Vec4Graphics& vec4);
	shaderResourceLocation Set(const std::string& uniformName, const Vec3Graphics& vec3);
	shaderResourceLocation Set(const std::string& uniformName, const Vec2Graphics& vec2);
	shaderResourceLocation Set(const std::string& uniformName, float f);
	shaderResourceLocation Set(const std::string& uniformName, int i);

	void Set(const shaderResourceLocation& uniformLocation, const Mat4Graphics& mat4);
	void Set(const shaderResourceLocation& uniformLocation, const Vec4Graphics& vec4);
	void Set(const shaderResourceLocation& uniformLocation, const Vec3Graphics& vec3);
	void Set(const shaderResourceLocation& uniformLocation, const Vec2Graphics& vec2);
	void Set(const shaderResourceLocation& uniformLocation, float f);
	void Set(const shaderResourceLocation& uniformLocation, int i);

	enum shadowType shadowType;
	Mat4Graphics shadowBias;
};

