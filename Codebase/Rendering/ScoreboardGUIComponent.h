#pragma once
#include "GUIComponent.h"
#include "GameObject.h"
#include "Font.h"

class ScoreboardGUIComponent : public GUIComponent
{
public:
	ScoreboardGUIComponent(Material* material, Texture* texture, float z, bool visible = true);
	virtual ~ScoreboardGUIComponent();

//	virtual void Update();
	virtual void Render();

	void Update(int scoreA, int scoreB, float time);

protected:
	Font* font;
	RenderComponent* renderComp;
	Mesh* m_NewMesh;

	int timeRem;
	int min;
	int sec;
};