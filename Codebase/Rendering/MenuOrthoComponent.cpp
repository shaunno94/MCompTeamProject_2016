#include "MenuOrthoComponent.h"

MenuOrthoComponent::MenuOrthoComponent(ButtonGUI* single, ButtonGUI* multi, ButtonGUI* exit, float z) : OrthoComponent(z)
{
	m_SingleBtn = single;
	m_MultiBtn = multi;
	m_ExitBtn = exit;

	//m_SelectTex = Texture::Get(TEXTURE_DIR"orange.png");

	//m_SingleBtn = new GameObject();
	//m_MultiBtn = new GameObject();
	//m_ExitBtn = new GameObject();

	//m_SingleMesh = Mesh::GenerateQuad();
	//m_MultiMesh = Mesh::GenerateQuad();
	//m_ExitMesh = Mesh::GenerateQuad();

	//m_SingleBtn->SetRenderComponent(new RenderComponent(m_Material, m_SingleMesh));
	//m_MultiBtn->SetRenderComponent(new RenderComponent(m_Material, m_MultiMesh));
	//m_ExitBtn->SetRenderComponent(new RenderComponent(m_Material, m_ExitMesh));
	//
	//m_SingleBtn->SetWorldTransform(Mat4Graphics::Translation(Vec3Graphics(-0.7f, 0.1f, 0)) * Mat4Graphics::Scale(Vec3Graphics(0.2f, 0.1f, 1)));
	//m_MultiBtn->SetWorldTransform(Mat4Graphics::Translation(Vec3Graphics(-0.7f, 0.4f, 0)) * Mat4Graphics::Scale(Vec3Graphics(0.2f, 0.1f, 1)));
	//m_ExitBtn->SetWorldTransform(Mat4Graphics::Translation(Vec3Graphics(-0.7f, 0.7f, 0)) * Mat4Graphics::Scale(Vec3Graphics(0.2f, 0.1f, 1)));

	//m_Children.push_back(m_SingleBtn);
	//m_Children.push_back(m_MultiBtn);
	//m_Children.push_back(m_ExitBtn);
}

MenuOrthoComponent::~MenuOrthoComponent()
{
	delete m_SingleBtn;
	delete m_MultiBtn;
	delete m_ExitBtn;
}

void MenuOrthoComponent::Update()
{
	m_SingleBtn->SetSelected(false);
	m_MultiBtn->SetSelected(false);
	m_ExitBtn->SetSelected(false);

	switch (m_Selection)
	{
		case SINGLE_PLAYER:
			m_SingleBtn->SetSelected(true);
			break;
		case MULTIPLAYER:
			m_MultiBtn->SetSelected(true);
			break;
		case QUIT:
			m_ExitBtn->SetSelected(true);
			break;
	}
}

void MenuOrthoComponent::Render()
{
	m_SingleBtn->Render();
	m_MultiBtn->Render();
	m_ExitBtn->Render();
}