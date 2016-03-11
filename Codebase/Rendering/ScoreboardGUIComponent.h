#pragma once
#include "GUIComponent.h"
#include "GameObject.h"
#include "Font.h"
#include "TextGUIComponent.h"

class ScoreboardGUIComponent : public TextGUIComponent
{
public:
	ScoreboardGUIComponent(Material* material, const std::string& text, Vec3Graphics pos, Vec3Graphics scale, bool visible = true);
	virtual ~ScoreboardGUIComponent();

	void Update(int scoreA, int scoreB, float time);

protected:

	int timeRem;
	int min;
	int sec;
};