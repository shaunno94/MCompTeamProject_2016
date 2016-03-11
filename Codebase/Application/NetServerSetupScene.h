#ifndef ORBIS
#pragma once
#include "Rendering\Scene.h"
#include "Rendering\UIControllerManager.h"
#include "Rendering\GUISystem.h"
#include "Rendering\OGLShader.h"
#include "Rendering\MenuBackgroundGUI.h"
#include "Rendering\MenuOrthoComponent.h"
#include "Networking\Net.h"
#include "Networking\NetServer.h"
#include "Rendering\TextGUIComponent.h"


#ifndef ORBIS
const string MENU_SERVER_VERT = SHADER_DIR"TexturedVertex.glsl";
const string MENU_SERVER_FRAG = SHADER_DIR"TexturedFragment.glsl";
#endif

class NetServerSetupScene :
	public Scene
{
public:
	NetServerSetupScene();
	~NetServerSetupScene();

	virtual void Setup() override;
	virtual void Cleanup() override;
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

	GUISystem* guiSystem;

	BaseShader* orthoShader;

	Material* guiMaterial;
	Material* textMaterial;
	Material* bgMaterial;
	Material* btnMaterial;
	Material* selectBtnMaterial;

	OrthoComponent* ipOrtho;
	OrthoComponent* connectionOrtho;
	MenuOrthoComponent* menuOrtho;

	MenuBackgroundGUI* menuBg;
	ButtonGUIComponent* startBtn;
	ButtonGUIComponent* cancelBtn;

	TextGUIComponent* ipAddress;
	TextGUIComponent* ipText;
	TextGUIComponent* sessionMemText;
	TextGUIComponent* startText;
	TextGUIComponent* cancelText;

	NetServer* server;
	std::vector<std::string*> ips;

	int m_Selected;
};
#endif
