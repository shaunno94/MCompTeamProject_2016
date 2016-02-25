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
		it->second->Load(textureUnit);
		Renderer::GetInstance()->UpdateUniform(it->first, textureUnit++);
	}

	//s_LastMaterialInUse = this;
}

int Material::Set(const std::string& uniformName, Texture* texture)
{
	return setUniformValue(m_uniformTextures, shader, uniformName, texture);
}

void Material::Set(int uniformLocation, Texture* texture)
{
	setUniformValue(m_uniformTextures, uniformLocation, texture);
}

void Material::UpdateUniform(int location, const Mat4Graphics& mat4)
{
	Renderer::GetInstance()->UpdateUniform(location, mat4);
}
void Material::UpdateUniform(int location, const Mat3Graphics& mat3)
{
	Renderer::GetInstance()->UpdateUniform(location, mat3);
}
void Material::UpdateUniform(int location, const Vec4Graphics& vec4)
{
	Renderer::GetInstance()->UpdateUniform(location, vec4);
}
void Material::UpdateUniform(int location, const Vec3Graphics& vec3)
{
	Renderer::GetInstance()->UpdateUniform(location, vec3);
}
void Material::UpdateUniform(int location, const Vec2Graphics& vec2)
{
	Renderer::GetInstance()->UpdateUniform(location, vec2);
}
void Material::UpdateUniform(int location, float f)
{
	Renderer::GetInstance()->UpdateUniform(location, f);
}
void Material::UpdateUniform(int location, double d)
{
	Renderer::GetInstance()->UpdateUniform(location, d);
}
void Material::UpdateUniform(int location, int i)
{
	Renderer::GetInstance()->UpdateUniform(location, i);
}
void Material::UpdateUniform(int location, unsigned int u)
{
	Renderer::GetInstance()->UpdateUniform(location, u);
}