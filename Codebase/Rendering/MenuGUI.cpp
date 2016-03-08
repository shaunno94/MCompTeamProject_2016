#include "MenuGUI.h"

MenuGUI::MenuGUI(Material* material, Texture* texture, float z, bool visible) : GUIComponent(material, texture,z, visible)
{
	m_Material = material;
	m_Texture = texture;
	m_Depth = z;
	m_Visible = visible;

	m_Texture->SetTextureParams(ANISOTROPIC_FILTERING);
	m_Material->Set(ReservedMeshTextures.DIFFUSE.name, m_Texture);

	m_SingleBtn = new GameObject();
	m_MultiBtn = new GameObject();
	m_ExitBtn = new GameObject();

	m_SingleMesh = Mesh::GenerateQuad();
	m_MultiMesh = Mesh::GenerateQuad();
	m_ExitMesh = Mesh::GenerateQuad();

	m_SingleBtn->SetRenderComponent(new RenderComponent(m_Material, m_SingleMesh));
	m_MultiBtn->SetRenderComponent(new RenderComponent(m_Material, m_MultiMesh));
	m_ExitBtn->SetRenderComponent(new RenderComponent(m_Material, m_ExitMesh));
	
	m_SingleBtn->SetWorldTransform(Mat4Graphics::Translation(Vec3Graphics(-0.6f, 0.1f, 0)) * Mat4Graphics::Scale(Vec3Graphics(0.2f, 0.1f, 1)));
	m_MultiBtn->SetWorldTransform(Mat4Graphics::Translation(Vec3Graphics(-0.6f, 0.4f, 0)) * Mat4Graphics::Scale(Vec3Graphics(0.2f, 0.1f, 1)));
	m_ExitBtn->SetWorldTransform(Mat4Graphics::Translation(Vec3Graphics(-0.6f, 0.7f, 0)) * Mat4Graphics::Scale(Vec3Graphics(0.2f, 0.1f, 1)));

	m_Children.push_back(m_SingleBtn);
	m_Children.push_back(m_MultiBtn);
	m_Children.push_back(m_ExitBtn);
}

MenuGUI::~MenuGUI()
{
	delete m_SingleBtn;
	delete m_MultiBtn;
	delete m_ExitBtn;

	delete m_SingleMesh;
	delete m_MultiMesh;
	delete m_ExitMesh;
}

void MenuGUI::Render()
{
	for (int i = 0; i < m_Children.size(); i++)
	{
		m_Children[i]->GetRenderComponent()->Draw();
	}
}