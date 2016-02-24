#include "ScoreboardGUI.h"

ScoreboardGUI::ScoreboardGUI()
:m_ScoreA(0),
 m_ScoreB(0),
 m_TimeRem(300)
{
	quad = new GameObject();
	quad->SetRenderComponent(new RenderComponent(new LightMaterial(new Shader(SHADER_DIR"combinevert.glsl", SHADER_DIR"combinefrag.glsl")), Mesh::GenerateQuad()));
}

ScoreboardGUI::~ScoreboardGUI()
{
	delete quad;
}

bool ScoreboardGUI::Update(int& score_a, int& score_b, int& timeRem, float dt)
{
	m_ScoreA = score_a;
	m_ScoreB = score_b;
	m_TimeRem = timeRem;
}