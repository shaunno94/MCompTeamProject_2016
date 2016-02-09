#pragma once
#include "Material.h"
class LightMaterial : public Material
{
protected:
	std::vector<std::pair<int, Vec4Graphics>> m_uniformVec4s;
	std::vector<std::pair<int, float>> m_uniformFloats;
	std::vector<std::pair<int, int>> m_uniformInts;

public:
	LightMaterial(Shader* shader, bool hasTranslucency = false);
	~LightMaterial();

	virtual void Setup() override;

	int Set(const std::string& uniformName, const Vec4Graphics& vec4);
	int Set(const std::string& uniformName, float f);
	int Set(const std::string& uniformName, int i);

	void Set(int uniformLocation, const Vec4Graphics& vec4);
	void Set(int uniformLocation, float f);
	void Set(int uniformLocation, int i);
};

