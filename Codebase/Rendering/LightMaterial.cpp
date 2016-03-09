#include "LightMaterial.h"
#include "Renderer.h"

LightMaterial::LightMaterial(BaseShader* shader, bool hasTranslucency) : Material(shader, hasTranslucency)
{
	shadowType = _NONE;
	Set(ReservedOtherTextures.DEPTH.name, (int)ReservedOtherTextures.DEPTH.index);
	Set(ReservedOtherTextures.NORMALS.name, (int)ReservedOtherTextures.NORMALS.index);
	Set(ReservedOtherTextures.SHADOW_2D.name, (int)ReservedOtherTextures.SHADOW_2D.index);
}


LightMaterial::~LightMaterial()
{
}

void LightMaterial::Setup()
{
	Material::Setup();
	UpdateUniformValue(m_uniformMat4s);
	UpdateUniformValue(m_uniformVec4s);
	UpdateUniformValue(m_uniformVec3s);
	UpdateUniformValue(m_uniformVec2s);
	UpdateUniformValue(m_uniformFloats);
	//This is hacky but...
#ifndef ORBIS
	UpdateUniformValue(m_uniformInts);
#endif
}

shaderResourceLocation LightMaterial::Set(const std::string& uniformName, const Mat4Graphics& mat4)
{
	return setUniformValue(m_uniformMat4s, shader, uniformName, mat4);
}

shaderResourceLocation LightMaterial::Set(const std::string& uniformName, const Vec4Graphics& vec4)
{
	return setUniformValue(m_uniformVec4s, shader, uniformName, vec4);
}

shaderResourceLocation LightMaterial::Set(const std::string& uniformName, const Vec3Graphics& vec3)
{
	return setUniformValue(m_uniformVec3s, shader, uniformName, vec3);
}

shaderResourceLocation LightMaterial::Set(const std::string& uniformName, const Vec2Graphics& vec2)
{
	return setUniformValue(m_uniformVec2s, shader, uniformName, vec2);
}

shaderResourceLocation LightMaterial::Set(const std::string& uniformName, float f)
{
	return setUniformValue(m_uniformFloats, shader, uniformName, f);
}

shaderResourceLocation LightMaterial::Set(const std::string& uniformName, int i)
{
	return setUniformValue(m_uniformInts, shader, uniformName, i);
}

void LightMaterial::Set(const shaderResourceLocation& uniformLocation, const Mat4Graphics& mat4)
{
	setUniformValue(m_uniformMat4s, uniformLocation, mat4);
}

void LightMaterial::Set(const shaderResourceLocation& uniformLocation, const Vec4Graphics& vec4)
{
	setUniformValue(m_uniformVec4s, uniformLocation, vec4);
}

void LightMaterial::Set(const shaderResourceLocation& uniformLocation, const Vec3Graphics& vec3)
{
	setUniformValue(m_uniformVec3s, uniformLocation, vec3);
}

void LightMaterial::Set(const shaderResourceLocation& uniformLocation, const Vec2Graphics& vec2)
{
	setUniformValue(m_uniformVec2s, uniformLocation, vec2);
}

void LightMaterial::Set(const shaderResourceLocation& uniformLocation, float f)
{
	setUniformValue(m_uniformFloats, uniformLocation, f);
}

void LightMaterial::Set(const shaderResourceLocation& uniformLocation, int i)
{
	setUniformValue(m_uniformInts, uniformLocation, i);
}