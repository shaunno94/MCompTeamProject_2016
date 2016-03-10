#include "Rendering/Renderer.h"
#include "Rendering/GameTimer.h"
#include "GameScene.h"
#include "MenuScene.h"
#include "Helpers/MeasuringTimer.h"
//#include "Networking\Net.h"

const float TIME_STEP = 1.0f / 120.0f;
const unsigned int SUB_STEPS = 4;

#ifndef ORBIS
const unsigned int SCREEN_HEIGHT = 800;
const unsigned int SCREEN_WIDTH = 1280;
#else
#include "Input\PS4Input.h"
#include "Rendering\PS4Controller.h"
const unsigned int SCREEN_HEIGHT = 1080;
const unsigned int SCREEN_WIDTH = 1920;
//System Variables
unsigned int sceLibcHeapExtendedAlloc = 1;			/* Switch to dynamic allocation */
size_t sceLibcHeapSize = 512 * 1024 * 1024;			/* Set up heap area upper limit as 256 MiB */
#endif

int main(void)
{
	//-------------------
	//--- MAIN Loop ---
	//-------------------

	//Initialise Renderer - including the window context if compiling for Windows - PC
	Renderer renderer("Team Project - 2016", SCREEN_WIDTH, SCREEN_HEIGHT, false);
	if (!renderer.HasInitialised())
	{
		return -1;
	}
	PhysicsEngineInstance::Instance();
	GameTimer timer;
#ifdef ORBIS
	PS4Input input = PS4Input();
#endif
	SoundSystem::Initialise();

	UIControllerManager* uiController = new UIControllerManager();
	vector<Scene*> scenes;
	//Create GameScene
	GameScene* gameScene = new GameScene();
	MenuScene* menuScene = new MenuScene(uiController);
	scenes.push_back(menuScene);
	scenes.push_back(gameScene);

	uiController->SetAvailableScene(scenes);
	
	//Set current scene to the game
	renderer.SetCurrentScene(menuScene);
	//renderer.SetCurrentScene(gameScene);

#ifdef _DEBUG
	std::cout << "Renderer Memory Usage: " << renderer.GetRendererMemUsage() / (1024 * 1024) << " (MB)" << std::endl;
	std::cout << "Texture Memory Usage: " << Texture::GetMemoryUsage() / (1024 * 1024) << " (MB)" << std::endl;
	std::cout << "Mesh Memory Usage: " << Mesh::GetMeshMemUsage() / (1024 * 1024) << " (MB)" << std::endl;
#endif

#ifndef ORBIS
	while (Window::GetWindow().UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE))
#else
	while (true)
#endif
	{
	
#ifdef ORBIS
		input.Poll();
#endif
		float ms = timer.GetTimer()->Get(1000.0f);

		MEASURING_TIMER_LOG_START("Physics");
		PhysicsEngineInstance::Instance()->stepSimulation(ms, SUB_STEPS, TIME_STEP);
		MEASURING_TIMER_LOG_END();

		uiController->update(ms);

		MEASURING_TIMER_LOG_START("Renderer");
		renderer.RenderScene(ms);
		MEASURING_TIMER_LOG_END();

		SoundSystem::Instance()->Update(ms);
		

		CLEAR_DEBUG_STREAM();
		MEASURING_TIMER_PRINT(GET_DEBUG_STREAM());

		MEASURING_TIMER_CLEAR();

		SoundSystem::Instance()->Update(ms);
	}

	SoundSystem::Instance()->Release();
	delete menuScene;
	delete gameScene;

	return 0;
}