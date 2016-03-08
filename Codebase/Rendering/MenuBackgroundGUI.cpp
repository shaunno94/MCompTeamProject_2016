#include "MenuBackgroundGUI.h"

MenuBackgroundGUI::MenuBackgroundGUI(Material* material, Texture* texture, float z, bool visible) : GUIComponent(material, texture, z, visible)
{
	m_Material = material;
	m_Texture = texture;
	m_Depth = z;
	m_Visible = visible;

	m_Material->Set(ReservedMeshTextures.DIFFUSE.name, m_Texture);

	background = new GameObject("bg");
	bg = Mesh::GenerateQuad();
	background->SetRenderComponent(new RenderComponent(m_Material, bg));
//	background->SetWorldTransform(Mat4Graphics::Translation(Vec3Graphics(350, 700, 0)) * Mat4Graphics::Scale(Vec3Graphics(40, 40, 1)));
	m_Children.push_back(background);
}

MenuBackgroundGUI::~MenuBackgroundGUI()
{

}


void MenuBackgroundGUI::Render()
{
	for (int i = 0; i < m_Children.size(); i++)
	{
		m_Children[i]->GetRenderComponent()->Draw();
	}

}

