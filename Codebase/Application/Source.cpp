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

	//Test Scenario - Tardis (cuboid collision shape), floor (cuboid collision shape), ball (sphere collison shape)
	Scene* myScene = new Scene();
	//Game objects added to scene are delete by the scene so don't delete twice.
	GameObject* ball = new GameObject("ball");
	GameObject* floor = new GameObject("floor");

	//Shader* simpleShader = new Shader(SHADER_DIR"textureVertex.glsl", SHADER_DIR"textureCoordFragment.glsl");
	Shader* simpleShader = new Shader(SHADER_DIR"textureVertex.glsl", SHADER_DIR"textureFragment.glsl");
	if (!simpleShader->IsOperational())
		return -1;
	Material* material = new Material(simpleShader);

	/*floor->SetRenderComponent(new RenderComponent(material, Mesh::GenerateQuad()));
	floor->CreateCollisionShape(0, Vec3Physics(0, 1, 0), true);
	floor->InitPhysics(0, Vec3Physics(0, -1, 0), QuatPhysics(1, 0, 0, 1));
	floor->SetLocalScale(Vec3Graphics(20.0f, 20.0f, 1.0f));
	myScene->addGameObject(floor);*/

	GameObject* tardis = new GameObject();
	tardis->SetRenderComponent(new RenderComponent(material, ModelLoader::LoadMGL(MODEL_DIR"Tardis/TARDIS.mgl", true)));
	tardis->CreateCollisionShape(Vec3Physics(5.0f, 1.0f, 5.0f), CUBOID);
	tardis->InitPhysics(0, Vec3Physics(0, 0, 0), QuatPhysics(0, 0, 0, 1));
	myScene->addGameObject(tardis);

	ball->SetRenderComponent(new RenderComponent(material, ModelLoader::LoadMGL(MODEL_DIR"Common/sphere.mgl", true)));
	ball->CreateCollisionShape(4.0);
	ball->InitPhysics(1.0, Vec3Physics(0, 14, 0), QuatPhysics(0, 0, 0, 1), Vec3Physics(1, 1, 1));
	myScene->addGameObject(ball);

	renderer.SetCurrentScene(myScene);

	while (Window::GetWindow().UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE))
	{
		PhysicsEngineInstance::Instance()->stepSimulation(TIME_STEP, SUB_STEPS);
		renderer.RenderScene(Window::GetWindow().GetTimer()->Get(1000.0f));
	}
	
	//Cleanup
	PhysicsEngineInstance::Release();
	Window::Destroy();
	delete myScene;
	return 0;
}