#include <cstdio>
#include <nclgl\Window.h>
#include "MyScene.h"
#include <ncltech\PhysicsEngine.h>
#include <ncltech\NCLDebug.h>
#include <future>
#include "ncltech\CommonMeshes.h"
#include <btBulletDynamicsCommon.h>

Scene* scene = NULL;

int Quit(bool pause = false, const string& reason = "") {
	if (scene) {
		delete scene;
		scene = NULL;
	}

	Window::Destroy();

	if (pause) {
		std::cout << reason << std::endl;
		system("PAUSE");
	}

	return 0;
}

void GameLoop(Scene* scene) {
	while (Window::GetWindow().UpdateWindow() && !scene->GetEndScene()) {
		//Note that for the sake of simplicity, all calculations with time will be done in seconds (ms * 0.001)
		// this is to simplify physics, as I cant visualise how fast 0.02 meters per millisecond is.
		float dt = Window::GetWindow().GetTimer()->GetTimedMS() * 0.001f;	//How many milliseconds since last update?

		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_P))
			PhysicsEngine::Instance()->SetPaused(!PhysicsEngine::Instance()->IsPaused());

		scene->UpdateScene(dt);

		scene->RenderScene();
		scene->Present();
	}
}

int main() {
	//-------------------
	//--- MAIN ENGINE ---
	//-------------------

	//Initialise the Window
	if (!Window::Initialise("Game Technologies - Framework Example", 1280, 800, false))
		return Quit(true, "Window failed to initialise!");

	Window::GetWindow().LockMouseToWindow(true);
	Window::GetWindow().ShowOSPointer(false);

	//Initialise the PhysicsEngine
	PhysicsEngine::Instance()->SetSpacePartition(new OctreeSpacePartition(Vec3Graphics(), Vec3Graphics(70.0f, 70.0f, 70.0f)));

	//Initialise the Scene
	scene = new MyScene(Window::GetWindow());
	if (!scene->HasInitialised())
		return Quit(true, "Renderer failed to initialise!");

	std::future<void> physicsEngineHandle = async(std::launch::async, PhysicsEngine::Run);
	Scene* previousScene = nullptr;

	PhysicsEngine::Instance()->PhysicsCycleCallback = [&]() {
		if (previousScene) {
			delete previousScene;
			previousScene = nullptr;
		}
	};
	btBroadphaseInterface* broadphase = new btDbvtBroadphase();
	CommonMeshes::InitializeMeshes();
	NCLDebug::LoadShaders();

	while (scene) {
		GameLoop(scene);
		scene->Cleanup();
		Scene* temp = scene->GetNextScene(Window::GetWindow());
		previousScene = scene;
		scene = temp;
	}

	//stop PhysicsEngine thread
	PhysicsEngine::g_Running = false;
	physicsEngineHandle.get();

	CommonMeshes::ReleaseMeshes();
	NCLDebug::ReleaseShaders();
	delete broadphase;
	//Cleanup
	return Quit();
}