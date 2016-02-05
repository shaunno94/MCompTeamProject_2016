#include <cstdio>
#include "Rendering\Window.h"
#include "Rendering\Renderer.h"
#include "PhysicsEngine\PhysicsEngineInstance.h"

const float TIME_STEP = 1.0f / 60.0f;
const unsigned int SUB_STEPS = 10;

int main() {
	//-------------------
	//--- MAIN ENGINE ---
	//-------------------

	//Initialise the Window
	if (!Window::Initialise("Game Technologies - Framework Example", 1280, 800, false)) {
		Window::Destroy();
		return -1;
	}

	Window::GetWindow().LockMouseToWindow(true);
	Window::GetWindow().ShowOSPointer(false);

	/*CommonMeshes::InitializeMeshes();
	NCLDebug::LoadShaders();*/

	Renderer renderer(Window::GetWindow());
	if (!renderer.HasInitialised()) {
		return -1;
	}

	//Initialise Bullet physics engine.
	PhysicsEngineInstance::Instance();
	PhysicsEngineInstance::Instance()->setGravity(btVector3(0, -9.81, 0));

	while (Window::GetWindow().UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE))
	{
		PhysicsEngineInstance::Instance()->stepSimulation(TIME_STEP, SUB_STEPS);
		renderer.UpdateScene(Window::GetWindow().GetTimer()->Get(1000.0f));
		renderer.RenderScene();
	}

	//Cleanup
	Window::Destroy();
	return 0;
}