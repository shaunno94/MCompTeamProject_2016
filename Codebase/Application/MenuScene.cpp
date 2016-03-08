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
	std::cout << menuGUI->GetSelection() << std::endl;
	menuGUI->Update();
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
	//textMaterial = new Material(orthoShader);
}

void MenuScene::DrawGUI()
{
	bgOrtho = new OrthoComponent(1.0f);
	menuBg = new MenuBackgroundGUI(bgMaterial, Texture::Get(TEXTURE_DIR"titleScreen1.jpg"), 1.0);
	bgOrtho->AddGUIComponent(menuBg);

	menuGUI = new MenuGUI(guiMaterial, Texture::Get(TEXTURE_DIR"bricks1.jpg"), 1.0,"menuGUI");
	bgOrtho->AddGUIComponent(menuGUI);
	GUISystem::GetInstance().AddOrthoComponent(bgOrtho);
}

void MenuScene::LoadAudio()
{

}

void MenuScene::SetupControls()
{
	myControllers->setProducer(menuGUI,0);
}




