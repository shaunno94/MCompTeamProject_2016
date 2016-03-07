#pragma once
#include "Rendering\Scene.h"
#include "Rendering\LocalControlManager.h"
#include "Rendering\GUISystem.h"

enum SELECTED
{
	SINGLE_PLAYER,
	MULTIPLAYER
};

class MenuScene :
	public Scene
{
public:
	MenuScene(ControllerManager* controller, bool selected = SINGLE_PLAYER);
	~MenuScene();

	void SetupShaders();
	void SetupMaterials();
	void DrawGUI();
	void LoadAudio();
	void SetupControls();

protected:
	ControllerManager* myControllers;
	bool m_Selected;
};

