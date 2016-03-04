#include "ScoreboardGUIComponent.h"

ScoreboardGUIComponent::ScoreboardGUIComponent(Material* material, Texture* texture, float z, bool visible, const std::string& name) : GUIComponent(material, texture, z, visible, name),
m_ScoreA(0),
m_ScoreB(0),
m_TimeRem(300)
{
	m_Material = material;
	m_Texture = texture;
	m_Depth = z;
	m_Visible = visible;

	m_Material->Set(ReservedMeshTextures.DIFFUSE.name, m_Texture);

	font = new Font(m_Texture, 16, 16);

	m_Mesh = Mesh::GenerateTextQuad("Team1: " + std::to_string(m_ScoreA) + "Team2: " + std::to_string(m_ScoreB), font);

	text = new GameObject("text");
	text_renderComp = new RenderComponent(m_Material, m_Mesh);
	text->SetRenderComponent(text_renderComp);

	text->SetWorldTransform(Mat4Graphics::Translation(Vec3Graphics(350,700, 0)) * Mat4Graphics::Scale(Vec3Graphics(40, 40, 1)));
}

ScoreboardGUIComponent::~ScoreboardGUIComponent()
{
	delete text;
}

void ScoreboardGUIComponent::Update()
{
	
}

void ScoreboardGUIComponent::Update(int scoreA, int scoreB)
{
	m_ScoreA = scoreA;
	m_ScoreB = scoreB;
	m_Mesh = Mesh::GenerateTextQuad("Team1: " + std::to_string(m_ScoreA) + "Team2: " + std::to_string(m_ScoreB), font);
	text_renderComp->m_Mesh = m_Mesh;
	text->SetRenderComponent(text_renderComp);
}

void ScoreboardGUIComponent::Render()
{
	text->GetRenderComponent()->Draw();
}

