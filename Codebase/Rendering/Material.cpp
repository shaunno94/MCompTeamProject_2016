#include "Material.h"

#include <cassert>
#include "constants.h"
#include "Renderer.h"


Material::Material(Shader* shader, bool hasTranslucency) : shader(shader), hasTranslucency(hasTranslucency)
{
	assert(("Material class constructor was given a non operational shader.", shader->IsOperational()));
}

Material::~Material(void) {}

void Material::Setup(Renderer* r)
{
	r->SetCurrentShader(shader);
	int textureUnit = MaxMeshTextureMapSlots;
	for(auto it = m_uniformTextures.begin(); it != m_uniformTextures.end(); ++it)
	{
		it->second->Load(textureUnit);
		r->UpdateUniform(it->first, textureUnit++);
	}
}

int Material::Set(const std::string& uniformName, Texture* texture)
{
	return setUniformValue(m_uniformTextures, shader, uniformName, texture);
}
int Material::Set(const std::string& uniformName, const Mat4Graphics& mat4)
{
	return setUniformValue(m_uniformMat4s, shader, uniformName, mat4);
}

void Material::Set(int uniformLocation, Texture* texture)
{
	setUniformValue(m_uniformTextures, uniformLocation, texture);
}
void Material::Set(int uniformLocation, const Mat4Graphics& mat4)
{
	setUniformValue(m_uniformMat4s, uniformLocation, mat4);
}