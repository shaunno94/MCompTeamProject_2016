#include <cstdio>
#include "Rendering\Window.h"


int main() {
	//-------------------
	//--- MAIN ENGINE ---
	//-------------------

	//Initialise the Window
	if (!Window::Initialise("Game Technologies - Framework Example", 1280, 800, false)) {
		Window::Destroy();
		return 0;
	}

	Window::GetWindow().LockMouseToWindow(true);
	Window::GetWindow().ShowOSPointer(false);

	/*CommonMeshes::InitializeMeshes();
	NCLDebug::LoadShaders();*/

	while (Window::GetWindow().UpdateWindow()) {
		//Note that for the sake of simplicity, all calculations with time will be done in seconds (ms * 0.001)
		// this is to simplify physics, as I cant visualise how fast 0.02 meters per millisecond is.
		float dt = Window::GetWindow().GetTimer()->Get();	//How many milliseconds since last update?

		//scene->RenderScene();
	}

	//Cleanup
	Window::Destroy();
	return 0;
}