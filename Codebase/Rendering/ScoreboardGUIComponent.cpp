#include "ScoreboardGUIComponent.h"

ScoreboardGUIComponent::ScoreboardGUIComponent(Material* material, Texture* texture, float z, bool visible) : GUIComponent(material, texture, z, visible)
{
	m_Material->Set(ReservedMeshTextures.DIFFUSE.name, m_Texture);

	font = new Font(m_Texture, 16, 16);

	m_Mesh = Mesh::GenerateTextQuad(std::to_string(0) + " - " + "3:00" + " - 0", font);
	renderComp = new RenderComponent(m_Material, m_Mesh);
	SetRenderComponent(renderComp);
	SetWorldTransform(Mat4Graphics::Translation(Vec3Graphics(-0.6f, 0.7f, 0)) * Mat4Graphics::Scale(Vec3Graphics(0.1f, 0.1f, 1)));
}

ScoreboardGUIComponent::~ScoreboardGUIComponent()
{
	delete renderComp;
}

void ScoreboardGUIComponent::Update(int scoreA, int scoreB, float time)
{
	delete m_NewMesh;
	timeRem = 180 - (int) time;
	min = timeRem / 60;
	sec = timeRem % 60;
	m_NewMesh = Mesh::GenerateTextQuad(std::to_string(scoreA) + " - " +
		std::to_string(min) + ":" + (sec < 10 ? "0" : "") + std::to_string(sec) + " - " +
									std::to_string(scoreB), font);

	renderComp->m_Mesh = m_NewMesh;
	SetRenderComponent(renderComp);
}

void ScoreboardGUIComponent::Render()
{
	//for (int i = 0; i < m_Children.size(); i++)
	//{
	//	m_Children[i]->GetRenderComponent()->Draw();
	//}
	GetRenderComponent()->Draw();
}

