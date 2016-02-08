#include "LightMaterial.h"


#include "Renderer.h"

LightMaterial::LightMaterial(Shader* shader, bool hasTranslucency) : Material(shader, hasTranslucency)
{
}


LightMaterial::~LightMaterial()
{
}

void LightMaterial::Setup()
{
	Material::Setup();
	UpdateUniformValue(m_uniformVec4s);
	UpdateUniformValue(m_uniformFloats);
}


int LightMaterial::Set(const std::string& uniformName, const Vec4Graphics& vec4)
{
	return setUniformValue(m_uniformVec4s, shader, uniformName, vec4);
}

int LightMaterial::Set(const std::string& uniformName, float f)
{
	return setUniformValue(m_uniformFloats, shader, uniformName, f);
}

void LightMaterial::Set(int uniformLocation, const Vec4Graphics& vec4)
{
	setUniformValue(m_uniformVec4s, uniformLocation, vec4);
}
void LightMaterial::Set(int uniformLocation, float f)
{
	setUniformValue(m_uniformFloats, uniformLocation, f);
}