#pragma once
#include "GUIComponent.h"
#include "GameObject.h"

class ScoreboardGUIComponent : public GUIComponent
{
public:
	ScoreboardGUIComponent(Material* material, Texture* texture, float z, const std::string& name = "");
	virtual ~ScoreboardGUIComponent();

	virtual void Update();
	virtual void Render();

protected:
	void Update(int scoreA, int scoreB, int timeRem);
	int m_ScoreA;
	int m_ScoreB;
	int m_TimeRem;
};