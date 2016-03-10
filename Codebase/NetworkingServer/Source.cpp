#include "Rendering/Renderer.h"
#include "Rendering/GameTimer.h"
#include "GameScene.h"
#include "Networking/NetServer.h"
#include "Helpers/MeasuringTimer.h"

#include "Rendering/NetServerControllerManager.h"


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
	Network::Init();

	//create client instance
	NetServer* server = new NetServer(5, 5, 5);

	//TODO:server->GetAddressStr() prints net info and does not just return the ip (returned ip is also not valid, but printed infor is)
	std::cout << "My address:"LINE_SEPARATOR_DEF;
	std::vector<std::string*> ips = server->GetIpStr();
	for (std::string* ip : ips)
	{
		std::cout << "\t" << *ip << LINE_SEPARATOR_STR;
		delete ip;
	}
	ips.clear();

	NetConnectionDataInternal* ballConnection = new NetConnectionDataInternal(nullptr);
	ballConnection->name[0] = 'b';
	ballConnection->name[1] = 'a';
	ballConnection->name[2] = 'l';
	ballConnection->name[3] = 'l';
	ballConnection->name[4] = 0;
	server->AddToSession(ballConnection);

	unsigned int input = 1;
	while (input != 0)
	{
		//Print waiting connections and session;
		size_t size = server->GetConnectionCount();
		std::cout << "Connections:" << LINE_SEPARATOR_STR;
		for (size_t i = 0; i < size; ++i)
		{
			NetConnectionDataInternal* connection = server->GetConnection(i);
			if (connection)
				std::cout << connection->GetAddressStr() << LINE_SEPARATOR_STR;
		}
		auto* connections = server->GetSession()->GetMembers();
		size_t playerCount = connections->size();
		std::cout << "Session members:" << LINE_SEPARATOR_STR;
		for (size_t i = 0; i < playerCount; ++i)
		{
			auto* connection = (*connections)[i];
			if (connection)
				std::cout << i << " : " << connection->GetAddressStr() << LINE_SEPARATOR_STR;
		}

		std::cout << "[0-8] to add connections to session; >=9 to continue"LINE_SEPARATOR_DEF;
		std::cin >> input;

		if (input < size && input >= 0) //add connection to session
			server->AddToSession(server->GetConnection(input));

		std::cout << "0 to start session; >0 to loop"LINE_SEPARATOR_DEF;
		std::cin >> input;
	}

	std::cout << "Waiting for other players"LINE_SEPARATOR_DEF;
	while (!server->StartSession()) //waiting for everyone to send the ready message
	{
	}
	std::cout << "Session starting"LINE_SEPARATOR_DEF;
	NetSession* session = server->GetSession();

	std::cout << "Setting up session..."LINE_SEPARATOR_DEF;
	while (session->GetState() != NetSessionState::NetSessionActive)
	{
	}

	std::cout << "Session started"LINE_SEPARATOR_DEF;
















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

	ControllerManager* myControllers = new NetServerControllerManager(session);
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

		MEASURING_TIMER_CLEAR();
	}

	delete gameScene;

	SoundSystem::Release();

	return 0;
}