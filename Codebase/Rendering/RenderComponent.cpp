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
	//Object is outside of view frustum
	if (disabled)
		return;

	Renderer::GetInstance()->SetCurrentShader(m_Material->GetShader());
	//model matrix
	Renderer::GetInstance()->UpdateUniform(m_Material->GetShader()->GetModelMatrixLocation(), m_GameObject->GetWorldTransform());
	//Inverse, transposed model matrix
	//if(m_Material->GetShader()->GetInverseModelMatrixLocation() >= 0)
	Renderer::GetInstance()->UpdateUniform(m_Material->GetShader()->GetInverseModelMatrixLocation(), Matrix4Simple::Inverse(Matrix4Simple::Transpose(m_GameObject->GetWorldTransform())));
	m_Mesh->Draw(m_Material);	
}

void RenderComponent::SetParent(GameObject* go)
{
	m_GameObject = go;
}