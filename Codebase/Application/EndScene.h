#pragma once
#include "Rendering\Scene.h"
#include "Rendering\UIControllerManager.h"
#include "Rendering\GUISystem.h"
#include "Rendering\OGLShader.h"
#include "Rendering\MenuBackgroundGUI.h"
#include "Rendering\MenuOrthoComponent.h"

#ifdef ORBIS
#include "Input\PS4Input.h"
#endif

enum team{
	RED_WINS,
	BLUE_WINS,
	TIE
};

class EndScene :
	public Scene
{
public:
	EndScene();
	~EndScene();

	virtual void UpdateScene(float dt) override;

	void Setup() override;
	void SetupGameObjects();
	void SetupShaders();
	void SetupMaterials();
	void DrawGUI();
	void LoadAudio();
	void SetupControls();
	void SetWinningTeam(team team){ winningTeam = team; }

	GUISystem* getGUISystem() override { return guiSystem; }

protected:
	UIControllerManager* myControllers;

	GameObject* test;
	GUISystem* guiSystem;


	BaseShader* orthoShader;

	Material* guiMaterial;
	Material* textMaterial;
	Material* bgMaterials[3];

	OrthoComponent* bgOrtho;
	MenuOrthoComponent* menuOrtho;

	MenuBackgroundGUI* background;

	team winningTeam;
};


