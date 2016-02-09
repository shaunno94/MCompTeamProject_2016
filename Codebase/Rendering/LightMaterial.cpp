#include "LightMaterial.h"


#include "Renderer.h"

LightMaterial::LightMaterial(Shader* shader, bool hasTranslucency) : Material(shader, hasTranslucency)
{
	Set(ReservedOtherTextures.DEPTH.name, (int)ReservedOtherTextures.DEPTH.index);
	Set(ReservedOtherTextures.NORMALS.name, (int)ReservedOtherTextures.NORMALS.index);
	//TODO-Set("pixelSize",)??

}


LightMaterial::~LightMaterial()
{
}

void LightMaterial::Setup()
{
	Material::Setup();
	UpdateUniformValue(m_uniformVec4s);
	UpdateUniformValue(m_uniformFloats);
	UpdateUniformValue(m_uniformInts);
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

void LightMaterial::Set(int uniformLocation, int i)
{
	setUniformValue(m_uniformInts, uniformLocation, i);
}