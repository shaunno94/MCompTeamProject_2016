#pragma once
#include "Rendering\Scene.h"
#include "Rendering\LocalControlManager.h"
#include "Rendering\GUISystem.h"
#include "Rendering\OGLShader.h"
#include "Rendering\MenuBackgroundGUI.h"


#ifndef ORBIS
const string MENU_VERT = SHADER_DIR"TexturedVertex.glsl";
const string MENU_FRAG = SHADER_DIR"TexturedFragment.glsl";
#endif

enum SELECTED
{
	SINGLE_PLAYER,
	MULTIPLAYER,
	QUIT
};

class MenuScene :
	public Scene
{
public:
	MenuScene(ControllerManager* controller, bool selected = SINGLE_PLAYER);
	~MenuScene();

	virtual void UpdateScene(float dt) override;

	void SetupGameObjects();
	void SetupShaders();
	void SetupMaterials();
	void DrawGUI();
	void LoadAudio();
	void SetupControls();

protected:
	ControllerManager* myControllers;

	GameObject* test;
	
	Mesh* bg;

	BaseShader* orthoShader;

	Material* guiMaterial;
	Material* textMaterial;

	OrthoComponent* bgOrtho;
	MenuBackgroundGUI* menuGUI;

	bool m_Selected;
};

