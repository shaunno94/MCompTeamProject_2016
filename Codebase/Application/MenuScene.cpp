#include "MenuScene.h"


MenuScene::MenuScene(ControllerManager* controller)
:myControllers(controller)
{
	SoundSystem::Initialise();
	GUISystem::Initialise();
	
	if (!GUISystem::GetInstance().HasInitialised())
	{
		std::cout << "GUI not Initialised!" << std::endl;
	}

	//SetupShaders();
	//SetupMaterials();
	//DrawGUI();
	//LoadAudio();
	//SetupControls();

}


MenuScene::~MenuScene()
{
}
