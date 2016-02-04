#include "Material.h"

#include <cassert>
#include "constants.h"


Material::Material(Shader* shader, bool hasTranslucency) : shader(shader), hasTranslucency(hasTranslucency), colour(Vec4Graphics::ONES) {
	assert(("Material class constructor was given a non operational shader.", shader->IsOperational()));
}

Material::~Material(void) {}


void Material::UpdateTextures(){
	for(auto it = uniformTextures.begin(); it != uniformTextures.end(); ++it) {
		it->second->Load();
		glUniform1i(it->first, it->second->GetTextureUnitIndex());
	}
}

void Material::AddTexture(const std::string& uniformName, Texture* texture) {
	int location = glGetUniformLocation(shader->GetProgram(), uniformName.c_str());
	if(location >= 0)
		uniformTextures.push_back(std::pair<int, Texture*>(location, texture));
}