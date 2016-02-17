#include "ExtendedMaterial.h"

#include "Renderer.h"

ExtendedMaterial::ExtendedMaterial(OGLShader* shader, bool hasTranslucency) : Material(shader, hasTranslucency)
{
}


ExtendedMaterial::~ExtendedMaterial()
{
}

void ExtendedMaterial::Setup()
{
	Material::Setup();
	UpdateUniformValue(m_uniformMat4s);
	UpdateUniformValue(m_uniformMat3s);
	UpdateUniformValue(m_uniformVec4s);
	UpdateUniformValue(m_uniformVec3s);
	UpdateUniformValue(m_uniformVec2s);
	UpdateUniformValue(m_uniformFloats);
	UpdateUniformValue(m_uniformInts);
}

int ExtendedMaterial::Set(const std::string& uniformName, const Mat4Graphics& mat4)
{
	return setUniformValue(m_uniformMat4s, shader, uniformName, mat4);
}
int ExtendedMaterial::Set(const std::string& uniformName, const Mat3Graphics& mat3)
{
	return setUniformValue(m_uniformMat3s, shader, uniformName, mat3);
}
int ExtendedMaterial::Set(const std::string& uniformName, const Vec4Graphics& vec4)
{
	return setUniformValue(m_uniformVec4s, shader, uniformName, vec4);
}
int ExtendedMaterial::Set(const std::string& uniformName, const Vec3Graphics& vec3)
{
	return setUniformValue(m_uniformVec3s, shader, uniformName, vec3);
}
int ExtendedMaterial::Set(const std::string& uniformName, const Vec2Graphics& vec2)
{
	return setUniformValue(m_uniformVec2s, shader, uniformName, vec2);
}
int ExtendedMaterial::Set(const std::string& uniformName, float f)
{
	return setUniformValue(m_uniformFloats, shader, uniformName, f);
}
int ExtendedMaterial::Set(const std::string& uniformName, int i)
{
	return setUniformValue(m_uniformInts, shader, uniformName, i);
}

void ExtendedMaterial::Set(int uniformLocation, const Mat4Graphics& mat4)
{
	setUniformValue(m_uniformMat4s, uniformLocation, mat4);
}
void ExtendedMaterial::Set(int uniformLocation, const Mat3Graphics& mat3)
{
	setUniformValue(m_uniformMat3s, uniformLocation, mat3);
}
void ExtendedMaterial::Set(int uniformLocation, const Vec4Graphics& vec4)
{
	setUniformValue(m_uniformVec4s, uniformLocation, vec4);
}
void ExtendedMaterial::Set(int uniformLocation, const Vec3Graphics& vec3)
{
	setUniformValue(m_uniformVec3s, uniformLocation, vec3);
}
void ExtendedMaterial::Set(int uniformLocation, const Vec2Graphics& vec2)
{
	setUniformValue(m_uniformVec2s, uniformLocation, vec2);
}
void ExtendedMaterial::Set(int uniformLocation, float f)
{
	setUniformValue(m_uniformFloats, uniformLocation, f);
}
void ExtendedMaterial::Set(int uniformLocation, int i)
{
	setUniformValue(m_uniformInts, uniformLocation, i);
}