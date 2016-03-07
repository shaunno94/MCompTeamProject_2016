#include "Rendering/Renderer.h"
#include "Rendering/GameTimer.h"
#include "GameScene.h"
#include "Networking/NetClient.h"

#include "Rendering/NetClientControllerManager.h"


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
	//Initialize the networking settings
	Network::Init();

	//create client instance
	NetClient* client = new NetClient();

	bool connected = false;
	while (!connected)
	{
		std::cout << "Give Server address:"LINE_SEPARATOR_DEF;
		std::string addressStr;
		std::cin >> addressStr;

		//try to connect to an address
		client->ConnectToServer(addressStr);
		//get back connection handle
		auto connection = client->GetConnection();

		std::cout << "Waiting for the connection to finish:"LINE_SEPARATOR_DEF;
		//wait for the connection to finish
		while (connection->GetState() != NetPeerDisconnected && connection->GetState() != NetPeerConnected)
		{
		}

		switch (connection->GetState())
		{
		case NetPeerConnected:
			std::cout << "Connection successful!"LINE_SEPARATOR_DEF;
			connected = true;
			break;
		default:
			std::cout << "Connection failed!"LINE_SEPARATOR_DEF;
			break;
		}
	}

	std::cout << "Waiting to be accepted to a session"LINE_SEPARATOR_DEF;
	while (!client->GetSession())
	{
	}
	std::cout << "Session joined"LINE_SEPARATOR_DEF;


	std::cout << "Enter any number to send a ready message"LINE_SEPARATOR_DEF;
	int x;
	std::cin >> x;
	client->Ready();

	std::cout << "Waiting for the session to start"LINE_SEPARATOR_DEF;
	NetSession* session = client->GetSession();
	while (session->GetState() != NetSessionState::NetSessionActive)
	{
	}




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

	ControllerManager* myControllers = new NetClientControllerManager(session);
	//Create GameScene
	GameScene* gameScene = new GameScene(myControllers);
	//Set current scene to the game
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
		SoundSystem::Instance()->Update(ms);
	}

	delete gameScene;

	SoundSystem::Release();

	return 0;
}