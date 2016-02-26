#include "CubeMaterial.h"
#include "Renderer.h"

CubeMaterial::CubeMaterial(BaseShader* shader, bool hasTranslucency) : Material(shader, hasTranslucency)
{
}


CubeMaterial::~CubeMaterial()
{
}

void CubeMaterial::Setup()
{
	Material::Setup();
	UpdateUniformValue(m_uniformInts);
}

int CubeMaterial::Set(const std::string& uniformName, int i)
{
	return setUniformValue(m_uniformInts, shader, uniformName, i);
}

void CubeMaterial::Set(int uniformLocation, int i)
{
	setUniformValue(m_uniformInts, uniformLocation, i);
}