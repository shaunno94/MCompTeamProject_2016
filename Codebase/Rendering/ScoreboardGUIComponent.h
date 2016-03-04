#pragma once
#include "GUIComponent.h"
#include "GameObject.h"
#include "Font.h"

class ScoreboardGUIComponent : public GUIComponent
{
public:
	ScoreboardGUIComponent(Material* material, Texture* texture, float z, bool visible, const std::string& name);
	virtual ~ScoreboardGUIComponent();

	virtual void Update();
	virtual void Render();

	void Update(int scoreA, int scoreB);

protected:
	int m_ScoreA;
	int m_ScoreB;
	int m_TimeRem;

	GameObject* text;
	RenderComponent* text_renderComp;

	Font* font;
};