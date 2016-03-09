#include "MenuScene.h"


MenuScene::MenuScene(UIControllerManager* controller)
	:myControllers(controller)
{
	SoundSystem::Initialise();
	GUISystem::Initialise();

	if (!GUISystem::GetInstance().HasInitialised())
	{
		std::cout << "GUI not Initialised!" << std::endl;
	}


	SetupShaders();
	SetupMaterials();
	SetupGameObjects();
	DrawGUI();
	LoadAudio();
	SetupControls();
}


MenuScene::~MenuScene()
{
	SoundSystem::Release();
	GUISystem::Destroy();
}


void MenuScene::UpdateScene(float dt)
{
	//std::cout << menuOrtho->GetSelection() << std::endl;
	m_Selected = menuOrtho->Update();
}

void MenuScene::SetupGameObjects()
{

}

void MenuScene::SetupShaders()
{
	orthoShader = new OGLShader(MENU_VERT, MENU_FRAG);

	if (!orthoShader->IsOperational())
		std::cout << "Shader not opertational!" << std::endl;
}

void MenuScene::SetupMaterials()
{
	guiMaterial = new Material(orthoShader);
	bgMaterial = new Material(orthoShader);
	btnMaterial = new Material(orthoShader);
	selectBtnMaterial = new Material(orthoShader);
	//textMaterial = new Material(orthoShader);
}

void MenuScene::DrawGUI()
{
	bgOrtho = new OrthoComponent(1.0f);
	menuBg = new MenuBackgroundGUI(bgMaterial,  1.0);
	bgOrtho->AddGUIComponent(menuBg);

	menuOrtho = new MenuOrthoComponent(0.5);
	singleBtn = new ButtonGUIComponent(btnMaterial, selectBtnMaterial, Vec3Graphics(-0.7f, 0.7f, 0), Vec2Graphics(0.2f, 0.1f));
	multiBtn = new ButtonGUIComponent(btnMaterial, selectBtnMaterial, Vec3Graphics(-0.7f, 0.4f, 0), Vec2Graphics(0.2f, 0.1f));
	exitBtn = new ButtonGUIComponent(btnMaterial, selectBtnMaterial, Vec3Graphics(-0.7f, 0.1f, 0), Vec2Graphics(0.2f, 0.1f));

	menuOrtho->AddGUIComponent(singleBtn);
	menuOrtho->AddGUIComponent(multiBtn);
	menuOrtho->AddGUIComponent(exitBtn);

	GUISystem::GetInstance().AddOrthoComponent(bgOrtho);
	GUISystem::GetInstance().AddOrthoComponent(menuOrtho);
}

void MenuScene::LoadAudio()
{

}

void MenuScene::SetupControls()
{
	myControllers->setProducer(menuOrtho,0);
}




