#include "TextGUIComponent.h"

TextGUIComponent::TextGUIComponent(Material* material, const std::string& text, Vec3Graphics pos, Vec3Graphics scale, bool visible) : GUIComponent(material, visible)
{
	m_Texture = Texture::Get(TEXTURE_DIR"tahoma.tga");
	m_Texture->SetTextureParams(ANISOTROPIC_FILTERING);
	m_Material->Set(ReservedMeshTextures.DIFFUSE.name, m_Texture);

	m_Font = new Font(m_Texture, 16, 16);

	m_Mesh = Mesh::GenerateTextQuad(text, m_Font);
	m_RenderComp = new RenderComponent(m_Material, m_Mesh);
	SetRenderComponent(m_RenderComp);
	SetWorldTransform(Mat4Graphics::Translation(pos) * Mat4Graphics::Scale(scale));
}

TextGUIComponent::~TextGUIComponent()
{
	if (m_Font)
	{
		delete m_Font;
		m_Font = nullptr;
	}
	if (m_Mesh)
	{
		delete m_Mesh;
		m_Mesh = nullptr;
	}
}

void TextGUIComponent::Update(const std::string& text)
{
	if (m_Mesh)
	{
		delete m_Mesh;
		m_Mesh = nullptr;
	}
	
	m_Mesh = Mesh::GenerateTextQuad(text, m_Font);

	m_RenderComp->m_Mesh = m_Mesh;
	SetRenderComponent(m_RenderComp);
}
