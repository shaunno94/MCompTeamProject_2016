#pragma once
#include "Rendering\Scene.h"
#include "Rendering\UIControllerManager.h"
#include "Rendering\GUISystem.h"
#include "Rendering\OGLShader.h"
#include "Rendering\MenuBackgroundGUI.h"
#include "Rendering\MenuOrthoComponent.h"
#include "Rendering\TextGUIComponent.h"
#include "Rendering\ExtendedMaterial.h"
#include "Rendering\LightMaterial.h"
#include "constants.h"


class MenuScene :
	public Scene
{
public:
	MenuScene();
	~MenuScene();

	virtual void UpdateScene(float dt) override;

	void SetupGameObjects();
	void SetupShaders();
	void SetupMaterials();
	void DrawGUI();
	void LoadAudio();
	void SetupControls();

	GUISystem* getGUISystem(){ return guiSystem; }

	virtual void Cleanup() override;

	virtual void Setup() override;


protected:
	UIControllerManager* myControllers;

	GameObject* player;
	GameObject* stadium;
	GameObject* ball;
	GameObject* light2;

	GUISystem* guiSystem;

	BaseShader* simpleShader;
	BaseShader* colourShader;
	BaseShader* pointlightShader;
	BaseShader* orthoShader;

	Material* guiMaterial;
	Material* textMaterial;
	Material* bgMaterial;
	Material* btnMaterial;
	Material* selectBtnMaterial;

	LightMaterial* lightMaterial;

	Material* material;
	Material* netMaterial;
	Material* ballMaterial;
	ExtendedMaterial* redPostMaterial;
	ExtendedMaterial* bluePostMaterial;

	OrthoComponent* bgOrtho;
	MenuOrthoComponent* menuOrtho;
	OrthoComponent* textOrtho;

	MenuBackgroundGUI* menuBg;
	ButtonGUIComponent* singleBtn;
	ButtonGUIComponent* hostGameBtn;
	ButtonGUIComponent* joinGameBtn;
	ButtonGUIComponent* exitBtn;
	MenuBackgroundGUI* titleImg;

	Mesh* lightMesh;

	int m_Selected;

	float rotation;
	const Vec3Graphics playerTranslation = Vec3Graphics(100, 20, 0);
};

