#include "ScoreboardGUIComponent.h"

ScoreboardGUIComponent::ScoreboardGUIComponent(Material* material, Texture* texture, float z, bool visible) : GUIComponent(material, texture, z, visible)
{
	m_Material = material;
	m_Texture = texture;
	m_Depth = z;
	m_Visible = visible;

	m_Material->Set(ReservedMeshTextures.DIFFUSE.name, m_Texture);

	font = new Font(m_Texture, 16, 16);

	m_Mesh = Mesh::GenerateTextQuad(std::to_string(0) + " - " + "3:00" + " - 0", font);

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

void ScoreboardGUIComponent::Update(int scoreA, int scoreB, float time)
{
	float timeRem = 180 - time;
	int min = (int)timeRem / 60;
	int sec = (int)timeRem % 60;

	m_Mesh = Mesh::GenerateTextQuad(std::to_string(scoreA) + " - " + 
		std::to_string(min) + ":" + (sec < 10 ? "0" : "") + std::to_string(sec) + " - " +
									std::to_string(scoreB), font);

	text_renderComp->m_Mesh = m_Mesh;
	text->SetRenderComponent(text_renderComp);
}

void ScoreboardGUIComponent::Render()
{
	text->GetRenderComponent()->Draw();
}

