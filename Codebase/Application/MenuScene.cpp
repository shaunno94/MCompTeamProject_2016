#include "MenuScene.h"


MenuScene::MenuScene()
{
	myControllers = new UIControllerManager();

	guiSystem = new GUISystem();
	
	if (!guiSystem->HasInitialised())
	{
		std::cout << "GUI not Initialised!" << std::endl;
	}


	SetupShaders();
	SetupMaterials();
	SetupGameObjects();
	DrawGUI();
	SetupControls();
}


MenuScene::~MenuScene()
{
	delete btnMaterial;
	delete bgMaterial;
	delete guiMaterial;
	delete selectBtnMaterial;
	if (guiSystem)
		delete guiSystem;
}


void MenuScene::UpdateScene(float dt)
{
	myControllers->update(dt);
	m_Selected = menuOrtho->Update();
}

void MenuScene::SetupGameObjects()
{

}

void MenuScene::SetupShaders()
{
#ifndef ORBIS
	orthoShader = new OGLShader(MENU_VERT, MENU_FRAG);
#else

#endif

	if (!orthoShader->IsOperational())
		std::cout << "Shader not opertational!" << std::endl;
}

void MenuScene::SetupMaterials()
{
	guiMaterial = new Material(orthoShader);
	bgMaterial = new Material(orthoShader);
	btnMaterial = new Material(orthoShader);
	selectBtnMaterial = new Material(orthoShader);
	textMaterial = new Material(orthoShader);
}

void MenuScene::DrawGUI()
{
	menuOrtho = new MenuOrthoComponent(0);
	singleBtn = new ButtonGUIComponent(btnMaterial, selectBtnMaterial,   Vec3Graphics(-0.6f, 0.7f,0.0f), Vec2Graphics(0.3f, 0.05f));
	hostGameBtn = new ButtonGUIComponent(btnMaterial, selectBtnMaterial, Vec3Graphics(-0.6f, 0.5f, 0.0f), Vec2Graphics(0.3f, 0.05f));
	joinGameBtn = new ButtonGUIComponent(btnMaterial, selectBtnMaterial, Vec3Graphics(-0.6f, 0.3f, 0.0f), Vec2Graphics(0.3f, 0.05f));
	exitBtn = new ButtonGUIComponent(btnMaterial, selectBtnMaterial, Vec3Graphics(-0.6f, 0.1f, 0.0f), Vec2Graphics(0.3f, 0.05f));

	singleBtnText = new TextGUIComponent(textMaterial, "Single Player", Vec3Graphics(-0.9f, 0.7f, 0.0f), Vec3Graphics(0.04f, 0.04f, 1));
	hostGameBtnText = new TextGUIComponent(textMaterial, "Host Game", Vec3Graphics(-0.9f, 0.5f, 0.0f), Vec3Graphics(0.04f, 0.04f, 1));
	joinGameBtnText = new TextGUIComponent(textMaterial, "Join Game", Vec3Graphics(-0.9f, 0.3f, 0.0f), Vec3Graphics(0.04f, 0.04f, 1));
	exitBtnText = new TextGUIComponent(textMaterial, "Quit", Vec3Graphics(-0.9f, 0.1f, 0.0f), Vec3Graphics(0.04f, 0.04f, 1));

	singleBtn->AddChildObject(singleBtnText);
	hostGameBtn->AddChildObject(hostGameBtnText);
	joinGameBtn->AddChildObject(joinGameBtnText);
	exitBtn->AddChildObject(exitBtnText);

	menuOrtho->AddGUIComponent(singleBtn);
	menuOrtho->AddGUIComponent(hostGameBtn);
	menuOrtho->AddGUIComponent(joinGameBtn);
	menuOrtho->AddGUIComponent(exitBtn);

	guiSystem->AddOrthoComponent(menuOrtho);

}

void MenuScene::LoadAudio()
{

}

void MenuScene::SetupControls()
{
	myControllers->setProducer(menuOrtho,0);
}




