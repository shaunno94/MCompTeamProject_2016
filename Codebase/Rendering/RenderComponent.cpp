#include "RenderComponent.h"
#include "Renderer.h"

RenderComponent::RenderComponent(Material* material, Mesh* mesh)
{
	m_Material = material;
	m_Mesh = mesh;
	m_GameObject = nullptr;
	m_ModelMatrixLocation = -1;
}


RenderComponent::~RenderComponent()
{
}

//TODO: !!! Save available Mesh Texture locations on the Render Component !!!
void RenderComponent::Draw() {
	m_Material->Setup();
	Renderer::UpdateUniform(m_ModelMatrixLocation, m_GameObject->GetWorldTransform());
	m_Mesh->Draw();
}

void RenderComponent::SetParent(GameObject* go) {
	m_GameObject = go;
	//TODO: move modelMatrix string to constants header file
	m_ModelMatrixLocation = glGetUniformLocation(m_Material->GetShader()->GetProgram(), "modelMatrix");
}