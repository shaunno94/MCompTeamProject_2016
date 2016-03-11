#pragma once
#include "Rendering\Scene.h"
#include "Rendering\UIControllerManager.h"
#include "Rendering\GUISystem.h"
#include "Rendering\OGLShader.h"
#include "Rendering\MenuBackgroundGUI.h"
#include "Rendering\MenuOrthoComponent.h"


#ifndef ORBIS
const string END_VERT = SHADER_DIR"TexturedVertex.glsl";
const string END_FRAG = SHADER_DIR"TexturedFragment.glsl";
#else
#include "Input\PS4Input.h"
#endif

enum team{
	RED_TEAM,
	BLUE_TEAM,
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

	GUISystem* getGUISystem(){ return guiSystem; }

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


