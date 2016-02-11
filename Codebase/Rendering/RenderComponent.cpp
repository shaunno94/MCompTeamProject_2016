#include "RenderComponent.h"
#include "Renderer.h"

RenderComponent::RenderComponent(Material* material, Mesh* mesh)
{
	m_Material = material;
	m_Mesh = mesh;
	m_GameObject = nullptr;
}


RenderComponent::~RenderComponent()
{
}

//TODO: !!! Save available Mesh Texture locations on the Render Component !!!
void RenderComponent::Draw()
{
	Renderer::GetInstance()->SetCurrentShader(m_Material->GetShader());

	//model matrix
	Renderer::UpdateUniform(m_Material->GetShader()->GetModelMatrixUniformLocation(), m_GameObject->GetWorldTransform());

	//reset reserved mesh texture uniforms to use the right texture unit
	for (GLint i = 0; i < ReservedMeshTextures.size; ++i)
		Renderer::UpdateUniform(m_Material->GetShader()->GetReservedMeshTextureUniformLocation(i), i);

	m_Mesh->Draw(m_Material);
}

void RenderComponent::SetParent(GameObject* go)
{
	m_GameObject = go;
}