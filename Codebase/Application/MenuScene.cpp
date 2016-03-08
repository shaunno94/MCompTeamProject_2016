#include "MenuScene.h"


MenuScene::MenuScene(ControllerManager* controller, int selected)
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
	delete bg;
}


void MenuScene::UpdateScene(float dt)
{

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
	//textMaterial = new Material(orthoShader);
}

void MenuScene::DrawGUI()
{
	bgOrtho = new OrthoComponent(1.0f);
	//menuGUI = new MenuBackgroundGUI(guiMaterial, Texture::Get(TEXTURE_DIR"titleScreen.jpg"), 1.0);
	//bgOrtho->AddGUIComponent(menuGUI);

	menuGUI = new MenuGUI(guiMaterial, Texture::Get(TEXTURE_DIR"bricks1.jpg"), 1.0);
	bgOrtho->AddGUIComponent(menuGUI);
	GUISystem::GetInstance().AddOrthoComponent(bgOrtho);
}

void MenuScene::LoadAudio()
{

}

void MenuScene::SetupControls()
{
	myControllers->setMenuProducer(menuGUI,0);
}




