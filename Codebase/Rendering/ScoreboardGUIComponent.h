#pragma once
#include "GUIComponent.h"
#include "GameObject.h"
#include "Font.h"

class ScoreboardGUIComponent : public GUIComponent
{
public:
	ScoreboardGUIComponent(Material* material, bool visible = true);
	virtual ~ScoreboardGUIComponent();

//	virtual void Update();
	void Update(int scoreA, int scoreB, float time);

protected:
	Font* font;
	RenderComponent* renderComp;
	Mesh* m_Mesh;
	Mesh* m_NewMesh;
	Texture* m_Texture;

	int timeRem;
	int min;
	int sec;
};