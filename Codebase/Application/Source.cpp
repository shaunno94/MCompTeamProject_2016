#include <cstdio>
#include "Rendering\Renderer.h"
#include "Rendering\GameTimer.h"
#include "GameScene.h"

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
//int sceUserMainThreadPriority = SCE_KERNEL_DEFAULT_PRIORITY_USER;
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
	GameTimer timer;
#ifdef ORBIS
	PS4Input input = PS4Input();
#endif

	ControllerManager* myControllers = new LocalControlManager;
	GameScene* gameScene = new GameScene(myControllers);

	renderer.SetCurrentScene(gameScene);
	gameScene->SetControllerActor();


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
		PhysicsEngineInstance::Instance()->stepSimulation(ms, SUB_STEPS, TIME_STEP);
		myControllers->update(ms);
		renderer.RenderScene(ms);
	}

	//delete myScene;
	delete gameScene;
	return 0;
}