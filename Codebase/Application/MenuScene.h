#pragma once
#include "Rendering\Scene.h"
#include "Rendering\UIControllerManager.h"
#include "Rendering\GUISystem.h"
#include "Rendering\OGLShader.h"
#include "Rendering\MenuBackgroundGUI.h"
#include "Rendering\MenuOrthoComponent.h"


#ifndef ORBIS
const string MENU_VERT = SHADER_DIR"TexturedVertex.glsl";
const string MENU_FRAG = SHADER_DIR"TexturedFragment.glsl";
#endif

class MenuScene :
	public Scene
{
public:
	MenuScene(UIControllerManager* controller);
	~MenuScene();

	virtual void UpdateScene(float dt) override;

	void SetupGameObjects();
	void SetupShaders();
	void SetupMaterials();
	void DrawGUI();
	void LoadAudio();
	void SetupControls();

	GUISystem* getGUISystem(){ return guiSystem; }

protected:
	UIControllerManager* myControllers;

	GameObject* test;
	GUISystem* guiSystem;

	
	BaseShader* orthoShader;

	Material* guiMaterial;
	Material* textMaterial;
	Material* bgMaterial;
	Material* btnMaterial;
	Material* selectBtnMaterial;

	OrthoComponent* bgOrtho;
	MenuOrthoComponent* menuOrtho;

	MenuBackgroundGUI* menuBg;
	ButtonGUIComponent* singleBtn;
	ButtonGUIComponent* multiBtn;
	ButtonGUIComponent* exitBtn;

	int m_Selected;
};

