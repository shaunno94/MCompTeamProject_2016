#pragma once
#include "Material.h"
class CubeMaterial : public Material
{
public:
	CubeMaterial(BaseShader* shader, bool hasTranslucency = false);
	~CubeMaterial();

	virtual void Setup() override;

	shaderResourceLocation Set(const std::string& uniformName, int i);

	void Set(const shaderResourceLocation& uniformLocation, int i);

protected:
	std::vector<std::pair<shaderResourceLocation, int>> m_uniformInts;
};

