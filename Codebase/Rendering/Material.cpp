#include "Material.h"

#include <cassert>
#include "constants.h"
#include "Renderer.h"


Material* Material::s_LastMaterialInUse = nullptr;

Material::Material(BaseShader* shader, bool hasTranslucency /* = false */) : shader(shader), hasTranslucency(hasTranslucency)
{
	//Set(ReservedOtherTextures.DEPTH.name, ReservedOtherTextures.DEPTH.index);
	assert(("Material class constructor was given a non operational shader.", shader->IsOperational()));
}

Material::~Material(void) {}

void Material::Setup()
{
	//TODO: Removed for Reseting Mesh child defaults (might be put back if that feature is not needed)
	//if (s_LastMaterialInUse == this) return;

	Renderer::GetInstance()->UpdateShaderMatrices();
	int textureUnit = ReservedMeshTextures.size + ReservedOtherTextures.size;
	for(auto it = m_uniformTextures.begin(); it != m_uniformTextures.end(); ++it)
	{
#ifdef ORBIS
		it->second->Load(it->first);
#else
		shaderResourceLocation location;
		location.id = textureUnit;
		it->second->Load(location);
		Renderer::GetInstance()->UpdateUniform(it->first, textureUnit++);
#endif
	}

	//s_LastMaterialInUse = this;
}

shaderResourceLocation Material::Set(const std::string& uniformName, Texture* texture)
{
	return setUniformValue(m_uniformTextures, shader, uniformName, texture);
}

void Material::Set(const shaderResourceLocation& uniformLocation, Texture* texture)
{
	setUniformValue(m_uniformTextures, uniformLocation, texture);
}

void Material::UpdateUniform(const shaderResourceLocation& location, const Mat4Graphics& mat4)
{
	Renderer::GetInstance()->UpdateUniform(location, mat4);
}
void Material::UpdateUniform(const shaderResourceLocation& location, const Mat3Graphics& mat3)
{
	Renderer::GetInstance()->UpdateUniform(location, mat3);
}
void Material::UpdateUniform(const shaderResourceLocation& location, const Vec4Graphics& vec4)
{
	Renderer::GetInstance()->UpdateUniform(location, vec4);
}
void Material::UpdateUniform(const shaderResourceLocation& location, const Vec3Graphics& vec3)
{
	Renderer::GetInstance()->UpdateUniform(location, vec3);
}
void Material::UpdateUniform(const shaderResourceLocation& location, const Vec2Graphics& vec2)
{
	Renderer::GetInstance()->UpdateUniform(location, vec2);
}
void Material::UpdateUniform(const shaderResourceLocation& location, float f)
{
	Renderer::GetInstance()->UpdateUniform(location, f);
}
void Material::UpdateUniform(const shaderResourceLocation& location, double d)
{
	Renderer::GetInstance()->UpdateUniform(location, d);
}
void Material::UpdateUniform(const shaderResourceLocation& location, int i)
{
	Renderer::GetInstance()->UpdateUniform(location, i);
}
void Material::UpdateUniform(const shaderResourceLocation& location, unsigned int u)
{
	Renderer::GetInstance()->UpdateUniform(location, u);
}