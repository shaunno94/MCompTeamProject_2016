#pragma once
#include "Rendering\Scene.h"
#include "Rendering\LocalControlManager.h"
#include "Rendering\GUISystem.h"

class MenuScene :
	public Scene
{
public:
	MenuScene(ControllerManager* controller);
	~MenuScene();

	void SetupShaders();
	void SetupMaterials();
	void DrawGUI();
	void LoadAudio();
	void SetupControls();

protected:
	ControllerManager* myControllers;
};

