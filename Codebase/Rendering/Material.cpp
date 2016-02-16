#include "Material.h"

#include <cassert>
#include "constants.h"
#include "Renderer.h"


Material* Material::s_LastMaterialInUse = nullptr;

Material::Material(Shader* shader, bool hasTranslucency /* = false */) : shader(shader), hasTranslucency(hasTranslucency)
{
	//Set(ReservedOtherTextures.DEPTH.name, ReservedOtherTextures.DEPTH.index);
	assert(("Material class constructor was given a non operational shader.", shader->IsOperational()));
}

Material::~Material(void) {}

void Material::Setup()
{
	//TODO: Removed for Reseting Mesh child defaults (might be put back if that feature is not needed)
	//if (s_LastMaterialInUse == this) return;

	Renderer::GetInstance()->SetCurrentShader(shader);
	Renderer::GetInstance()->UpdateShaderMatrices();
	GLint textureUnit = ReservedMeshTextures.size + ReservedOtherTextures.size;
	for(auto it = m_uniformTextures.begin(); it != m_uniformTextures.end(); ++it)
	{
		it->second->Load(textureUnit);
		Renderer::UpdateUniform(it->first, textureUnit++);
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