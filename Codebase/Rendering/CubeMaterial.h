#pragma once
#include "Material.h"
class CubeMaterial : public Material
{
public:
	CubeMaterial(BaseShader* shader, bool hasTranslucency = false);
	~CubeMaterial();

	virtual void Setup() override;

	int Set(const std::string& uniformName, int i);

	void Set(int uniformLocation, int i);

protected:
	std::vector<std::pair<int, int>> m_uniformInts;
};

