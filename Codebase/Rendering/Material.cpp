#include "Material.h"

#include <cassert>
#include "constants.h"


Material::Material(Shader* shader, bool hasTranslucency) : shader(shader), hasTranslucency(hasTranslucency) {
	assert(("Material class constructor was given a non operational shader.", shader->IsOperational()));
}

Material::~Material(void) {}

void Material::UpdateTextures(){
	for(auto it = m_uniformTextures.begin(); it != m_uniformTextures.end(); ++it) {
		it->second->Load();
		glUniform1i(it->first, it->second->GetTextureUnitIndex());
	}
}

int Material::Set(const std::string& uniformName, Texture* texture) {
	return setUniformValue(m_uniformTextures, shader, uniformName, texture);
}

void Material::Set(int uniformLocation, Texture* texture) {
	setUniformValue(m_uniformTextures, uniformLocation, texture);
}