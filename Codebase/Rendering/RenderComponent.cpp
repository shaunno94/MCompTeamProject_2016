#include "RenderComponent.h"
#include "Renderer.h"

RenderComponent::RenderComponent()
{
	m_GameObject = nullptr;
	m_ModelMatrixLocattion = -1;
}


RenderComponent::~RenderComponent()
{
}

void RenderComponent::Draw(Renderer* r) {
	m_material->Setup(r);

	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, (float*)&modelMatrix);
	r->UpdateUniform("modelMatrix")
}

void RenderComponent::SetParent(GameObject* go) {
	m_GameObject = go;
}