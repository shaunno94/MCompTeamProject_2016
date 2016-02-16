#pragma once
#include "Material.h"
enum shadowType{ _NONE, _2D/*, _CUBE*/ };

class LightMaterial : public Material
{
protected:
	std::vector<std::pair<int, Mat4Graphics>> m_uniformMat4s;
	std::vector<std::pair<int, Vec4Graphics>> m_uniformVec4s;
	std::vector<std::pair<int, Vec3Graphics>> m_uniformVec3s;
	std::vector<std::pair<int, Vec2Graphics>> m_uniformVec2s;
	std::vector<std::pair<int, float>> m_uniformFloats;
	std::vector<std::pair<int, int>> m_uniformInts;

	

public:
	LightMaterial(Shader* shader, bool hasTranslucency = false);
	~LightMaterial();

	virtual void Setup() override;

	int Set(const std::string& uniformName, const Mat4Graphics& mat4);
	int Set(const std::string& uniformName, const Vec4Graphics& vec4);
	int Set(const std::string& uniformName, const Vec3Graphics& vec3);
	int Set(const std::string& uniformName, const Vec2Graphics& vec2);
	int Set(const std::string& uniformName, float f);
	int Set(const std::string& uniformName, int i);

	void Set(int uniformLocation, const Mat4Graphics& mat4);
	void Set(int uniformLocation, const Vec4Graphics& vec4);
	void Set(int uniformLocation, const Vec3Graphics& vec3);
	void Set(int uniformLocation, const Vec2Graphics& vec2);
	void Set(int uniformLocation, float f);
	void Set(int uniformLocation, int i);

	enum shadowType shadowType;
	Mat4Graphics shadowBias;
};

