#include "MenuBackgroundGUI.h"

MenuBackgroundGUI::MenuBackgroundGUI(Material* material, bool visible) : GUIComponent(material, visible)
{
	m_Texture = Texture::Get(TEXTURE_DIR"titleScreen1.jpg");
	m_Material->Set(ReservedMeshTextures.DIFFUSE.name, m_Texture);

	m_Mesh = Mesh::GenerateQuad();
	SetRenderComponent(new RenderComponent(m_Material, m_Mesh));
}

MenuBackgroundGUI::~MenuBackgroundGUI()
{
	delete m_Mesh;
	m_Texture->ClearAll();
}


void MenuBackgroundGUI::Render()
{
	for (int i = 0; i < m_Children.size(); i++)
	{
		m_Children[i]->GetRenderComponent()->Draw();
	}

	GetRenderComponent()->Draw();

}

