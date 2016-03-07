#include "MenuScene.h"


MenuScene::MenuScene(ControllerManager* controller, bool selected)
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
