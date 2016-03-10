#include "ButtonGUIComponent.h"

ButtonGUIComponent::ButtonGUIComponent(Material* material, Material* matSelected, Vec3Graphics pos, Vec2Graphics scale, bool selected, bool visible) : GUIComponent(material, visible)
{
	m_MatSelected = matSelected;

	m_Selected = selected;

	m_Texture = Texture::Get(TEXTURE_DIR"bricks1.jpg");
	m_TexSelected = Texture::Get(TEXTURE_DIR"blue.png");
	m_Texture->SetTextureParams(ANISOTROPIC_FILTERING);
	m_Material->Set(ReservedMeshTextures.DIFFUSE.name, m_Texture);

	m_Mesh = Mesh::GenerateQuad();
	SetRenderComponent(new RenderComponent(m_Material, m_Mesh));
	SetWorldTransform(Mat4Graphics::Translation(Vec3Graphics(pos.x, pos.y, 0.5)) * Mat4Graphics::Scale(Vec3Graphics(scale.x, scale.y, 0)));

}

ButtonGUIComponent::~ButtonGUIComponent()
{
	delete m_Mesh;
	m_Texture->Clear();
	m_TexSelected->Clear();
}

void ButtonGUIComponent::Update()
{
	GetRenderComponent()->m_Material = m_Material;
	if (m_Selected)
	{
		GetRenderComponent()->m_Material = m_MatSelected;
	}
}
