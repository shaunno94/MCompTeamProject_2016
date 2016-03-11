#include "ScoreboardGUIComponent.h"

ScoreboardGUIComponent::ScoreboardGUIComponent(Material* material, const std::string& text, Vec3Graphics pos, Vec3Graphics scale, bool visible)
	: TextGUIComponent(material, text, pos, scale, visible)
{

}

ScoreboardGUIComponent::~ScoreboardGUIComponent()
{
}

void ScoreboardGUIComponent::Update(int scoreA, int scoreB, float time)
{
	if (m_Mesh)
	{
		delete m_Mesh;
		m_Mesh = nullptr;
	}

	timeRem = 180 - (int) time;
	min = timeRem / 60;
	sec = timeRem % 60;
	m_Mesh = Mesh::GenerateTextQuad(std::to_string(scoreA) + " - " +
	                                std::to_string(min) + ":" + (sec < 10 ? "0" : "") + std::to_string(sec) + " - " +
	                                std::to_string(scoreB), m_Font);

	m_RenderComp->m_Mesh = m_Mesh;
	SetRenderComponent(m_RenderComp);
}


