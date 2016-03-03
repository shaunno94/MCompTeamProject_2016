#include "ScoreboardGUIComponent.h"

ScoreboardGUIComponent::ScoreboardGUIComponent(Material* material, Texture* texture, float z, bool visible) : GUIComponent(material, texture, z, visible),
m_ScoreA(0),
m_ScoreB(0),
m_TimeRem(300),
{
	m_Material = material;
	m_Texture = texture;
	m_Depth = z;
	m_Visible = visible;

	m_Texture->SetTextureParams(REPEATING);
	m_Material->Set("diffuseTex", m_Texture);

	font = new Font(m_Texture, 16, 16);

	Vec3Graphics* points = new Vec3Graphics[4];
	Vec3Graphics start(0.0f, 0.43f, 0);
	Vec3Graphics end(0.31f, 0.5f, 0);

	points[0] = Vec3Graphics(start.x, start.y, start.z);
	points[1] = Vec3Graphics(start.x, end.y, start.z);
	points[2] = Vec3Graphics(end.x, end.y, end.z);
	points[3] = Vec3Graphics(end.x, start.y, end.z);

	m_Mesh = Mesh::GenerateQuad(points);
	

	//m_Mesh = Mesh::GenerateTextQuad("asas",font);

	this->SetRenderComponent(new RenderComponent(m_Material, m_Mesh)); 
}

ScoreboardGUIComponent::~ScoreboardGUIComponent()
{

}

void ScoreboardGUIComponent::Update()
{
	Update(0, 0, 50);
}

void ScoreboardGUIComponent::Update(int scoreA, int scoreB, int timeRem)
{
	m_ScoreA = scoreA;
	m_ScoreB = scoreB;
	m_TimeRem = timeRem;
}

void ScoreboardGUIComponent::Render()
{
	Update();
	for (auto child : m_Children)
	{
		child->Render();
	}

	this->GetRenderComponent()->Draw();

}

