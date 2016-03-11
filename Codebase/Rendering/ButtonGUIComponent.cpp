#include "ButtonGUIComponent.h"

ButtonGUIComponent::ButtonGUIComponent(Material* material, Material* matSelected, Vec3Graphics pos, Vec2Graphics scale, bool selected, bool visible) : GUIComponent(material, visible)
{
	m_MatSelected = matSelected;

	m_Selected = selected;
	m_pos = pos;
	m_scale = scale;

	m_Texture = Texture::Get(TEXTURE_DIR"button1.png");
	m_TexSelected = Texture::Get(TEXTURE_DIR"blue.png");
	m_Texture->SetTextureParams(ANISOTROPIC_FILTERING);
	m_Material->Set(ReservedMeshTextures.DIFFUSE.name, m_Texture);
	m_MatSelected->Set(ReservedMeshTextures.DIFFUSE.name, m_TexSelected);

	m_Mesh = Mesh::GenerateQuad();
	SetRenderComponent(new RenderComponent(m_Material, m_Mesh));
	SetWorldTransform(Mat4Graphics::Translation(Vec3Graphics(pos.x, pos.y, pos.z)) * Mat4Graphics::Scale(Vec3Graphics(scale.x, scale.y, 0)));

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

void ButtonGUIComponent::AddText(Material* textMaterial, const std::string& text)
{
	m_Text =  new TextGUIComponent(textMaterial, text, Vec3Graphics(m_pos.x - 0.3f, m_pos.y + 0.02f, 0.0f), Vec3Graphics(m_scale.x / 10, m_scale.y, 0));
	AddChildObject(m_Text);
}
