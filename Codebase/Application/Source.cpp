#include <cstdio>
#include "Rendering\Window.h"
#include "Rendering\ShadowRenderer.h"


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

	ShadowRenderer renderer(Window::GetWindow());
	if (!renderer.HasInitialised()) {
		return -1;
	}

	while (Window::GetWindow().UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)){
		renderer.UpdateScene(Window::GetWindow().GetTimer()->Get(1000.0f));
		renderer.RenderScene();
	}

	//Cleanup
	Window::Destroy();
	return 0;
}