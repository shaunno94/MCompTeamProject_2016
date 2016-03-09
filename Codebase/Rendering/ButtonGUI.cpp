#include "ButtonGUI.h"

ButtonGUI::ButtonGUI(Material* material, Material* matSelected, Texture* texture, Texture* texSelected,
	float z, bool selected, Vec4Graphics* pos, Vec2Graphics* scale, bool visible) : GUIComponent(material, texture, z, visible)
{
	m_MatSelected = matSelected;
	m_TexSelected = texSelected;

	m_Selected = selected;

	m_Texture->SetTextureParams(ANISOTROPIC_FILTERING);
	m_Material->Set(ReservedMeshTextures.DIFFUSE.name, m_Texture);

	m_Mesh = Mesh::GenerateQuad();
	SetRenderComponent(new RenderComponent(m_Material, m_Mesh));
	SetWorldTransform(Mat4Graphics::Translation(Vec3Graphics(pos->x, pos->y, 0)) * Mat4Graphics::Scale(Vec3Graphics(scale->x, scale->y, 0)));

}

ButtonGUI::~ButtonGUI()
{
	delete m_MatSelected;
}

void ButtonGUI::Update(bool select)
{
	m_Selected = select;
	m_Material->Set(ReservedMeshTextures.DIFFUSE.name, m_Texture);
	SetRenderComponent(new RenderComponent(m_Material, m_Mesh));

	if (m_Selected)
	{
		m_MatSelected->Set(ReservedMeshTextures.DIFFUSE.name, m_TexSelected);
		SetRenderComponent(new RenderComponent(m_MatSelected, m_Mesh));
	}
}

void ButtonGUI::Render()
{
	GetRenderComponent()->Draw();
}