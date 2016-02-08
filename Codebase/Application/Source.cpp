#include <cstdio>
#include "Rendering\Window.h"
#include "Rendering\Renderer.h"
#include "PhysicsEngine\PhysicsEngineInstance.h"
#include "Rendering/ModelLoader.h"

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
	PhysicsEngineInstance::Instance()->setGravity(btVector3(0, -9.81, 0));

	//Test Scenario - one 'icosphere' which will be affected by bullet physics
	Scene* myScene = new Scene();
	//Game objects added to scene are delete by the scene so don't delete twice.
	GameObject* test = new GameObject("test");
	GameObject* floor = new GameObject("floor");
	GameObject* cube = new GameObject();

	Shader* simpleShader = new Shader(SHADER_DIR"basicVertex.glsl", SHADER_DIR"colourFragment.glsl");
	if (!simpleShader->IsOperational())
		return -1;
	Material* material = new Material(simpleShader);

	floor->SetRenderComponent(new RenderComponent(material, Mesh::GenerateQuad()));
	floor->InitPhysics(0, Vec3Physics(0, 0, 0), QuatPhysics(1, 0, 0, 1), Vec3Physics(10, 0, 0));
	
	test->SetRenderComponent(new RenderComponent(material, Mesh::GenerateIcosphere(1)));
	test->InitPhysics();
	//myScene->addGameObject(test);
	myScene->addGameObject(floor);
	
	cube->SetRenderComponent(new RenderComponent(material, ModelLoader::LoadOBJ(MODEL_DIR"Tardis/TARDIS.obj", true)));
	cube->InitPhysics(0);
	//cube->AddChildObject(test);
	myScene->addGameObject(cube);

	renderer.SetCurrentScene(myScene);

	while (Window::GetWindow().UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE))
	{
		PhysicsEngineInstance::Instance()->stepSimulation(TIME_STEP, SUB_STEPS, TIME_STEP);
		renderer.RenderScene(Window::GetWindow().GetTimer()->Get(1000.0f));
	}
	
	//Cleanup
	PhysicsEngineInstance::Release();
	Window::Destroy();
	delete myScene;
	return 0;
}