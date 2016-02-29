#define STB_TRUETYPE_IMPLEMENTATION
#define _CRT_SECURE_NO_WARNINGS
#include "ScoreboardGUIComponent.h"
#include "Helpers\stb_truetype.h"

unsigned char ttf_buffer[1 << 20];
unsigned char temp_bitmap[512 * 512];
stbtt_bakedchar cdata[96]; // ASCII 32..126 is 95 glyphs
GLuint ftex;


ScoreboardGUIComponent::ScoreboardGUIComponent(Material* material, Texture* texture, float z, const std::string& name) : GUIComponent(material, texture, z),
	m_ScoreA(0),
	m_ScoreB(0),
	m_TimeRem(300)
{

	m_Name = name;
	m_Material = material;
	m_Texture = texture;
	depth = z;



	material->Set("diffuseTex", m_Texture);

	Vec3Graphics* points = new Vec3Graphics[4];
	Vec3Graphics start(0.25f, -0.6, 0);
	Vec3Graphics end(-0.25f, -1.0f, 0);
	points[0] = Vec3Graphics(start.x, start.y, start.z);
	points[1] = Vec3Graphics(start.x, end.y, start.z);
	points[2] = Vec3Graphics(end.x, end.y, end.z);
	points[3] = Vec3Graphics(end.x, start.y, end.z);

	float texLength = (start - end).Length();
	m_Mesh = Mesh::GenerateQuad(points, Vec2Graphics(texLength, texLength));
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
