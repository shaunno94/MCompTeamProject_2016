#include <cstdio>
#include "Rendering\Window.h"
#include "Rendering\Renderer.h"
#include "PhysicsEngine\PhysicsEngineInstance.h"
#include "Rendering\ModelLoader.h"
#include "Rendering\DebugDraw.h"
#include "Rendering\LightMaterial.h"

const float TIME_STEP = 1.0f / 120.0f;
const unsigned int SUB_STEPS = 2;

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

#if DEBUG_DRAW
	PhysicsEngineInstance::Instance()->setDebugDrawer(DebugDraw::Instance());
	DebugDraw::Context(&renderer);
#endif

	//Test Scenario - Tardis (cuboid collision shape), floor (plane collision shape), ball (sphere collison shape)
	Scene* myScene = new Scene();
	//Game objects added to scene are delete by the scene so don't delete twice.
	GameObject* ball = new GameObject("ball");
	GameObject* floor = new GameObject("floor");
	GameObject* light1 = new GameObject("l");
	GameObject* light2 = new GameObject("l");
	GameObject* tardis = new GameObject("tar");

	//Physics objects hold collision shape and collision object(body), 
	//call CreateCollisionShape before CreatePhysicsBody or the object will not be created correctly.
	//Physics objects will be deleted by the game object.
	RigidPhysicsObject* ballPhysics = new RigidPhysicsObject();
	ballPhysics->CreateCollisionShape(1.0);
	ballPhysics->CreatePhysicsBody(1.0, Vec3Physics(10, 20, 0), QuatPhysics(0, 0, 0, 1), Vec3Physics(1, 1, 1));

	RigidPhysicsObject* floorPhysics = new RigidPhysicsObject();
	floorPhysics->CreateCollisionShape(0, Vec3Physics(0, 1, 0), true);
	floorPhysics->CreatePhysicsBody(0, Vec3Physics(0, -1, 0), QuatPhysics(0, 0, 0, 1));

	RigidPhysicsObject* tardisPhysics = new RigidPhysicsObject();
	tardisPhysics->CreateCollisionShape(Vec3Physics(1.5f, 2.0f, 1.5f), CUBOID);
	tardisPhysics->CreatePhysicsBody(0, Vec3Physics(0, 0, 0), QuatPhysics(0, 0, 0, 1));

	Shader* simpleShader = new Shader(SHADER_DIR"textureVertex.glsl", SHADER_DIR"textureFragment.glsl");
	Shader* pointlightShader = new Shader(SHADER_DIR"pointlightvertex.glsl", SHADER_DIR"pointlightfragment.glsl");

	if (!pointlightShader->IsOperational())
		return -1;

	if (!simpleShader->IsOperational())
		return -1;

	LightMaterial* lightMaterial = new LightMaterial(pointlightShader);
	light1->SetRenderComponent(new RenderComponent(lightMaterial, ModelLoader::LoadMGL(MODEL_DIR"Common/ico.mgl", true)));
	light1->SetWorldTransform(Mat4Graphics::Translation(Vec3Graphics(0, 2, 2)) *Mat4Graphics::Scale(Vec3Graphics(20, 20, 20)));
	light1->SetBoundingRadius(20);

	light2->SetRenderComponent(new RenderComponent(lightMaterial, ModelLoader::LoadMGL(MODEL_DIR"Common/ico.mgl", true)));
	light2->SetWorldTransform(Mat4Graphics::Translation(Vec3Graphics(0, 40, 0)) *Mat4Graphics::Scale(Vec3Graphics(400, 400, 400)));
	light2->SetBoundingRadius(400);


	Material* material = new Material(simpleShader);

	floor->SetRenderComponent(new RenderComponent(material, Mesh::GenerateQuad()));
	floor->SetPhysicsComponent(floorPhysics);
	floor->SetLocalTransform(Mat4Graphics::Scale(Vec3Graphics(40, 40, 40)) * Mat4Graphics::Rotation(90.0f, Vec3Graphics(1, 0, 0)));
	myScene->addGameObject(floor);

	tardis->SetRenderComponent(new RenderComponent(material, ModelLoader::LoadMGL(MODEL_DIR"Tardis/TARDIS.mgl", true)));
	tardis->SetPhysicsComponent(tardisPhysics);
	myScene->addGameObject(tardis);
	myScene->addLightObject(light1);
	myScene->addLightObject(light2);

	ball->SetRenderComponent(new RenderComponent(material, ModelLoader::LoadMGL(MODEL_DIR"Common/sphere.mgl", true)));
	ball->SetPhysicsComponent(ballPhysics);
	myScene->addGameObject(ball);

	renderer.SetCurrentScene(myScene);

	while (Window::GetWindow().UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE))
	{
		float ms = Window::GetWindow().GetTimer()->Get(1000.0f);
		PhysicsEngineInstance::Instance()->stepSimulation(ms, SUB_STEPS, TIME_STEP);
		renderer.RenderScene(ms);
	}
	//Cleanup
	PhysicsEngineInstance::Release();
	DebugDraw::Release();
	Window::Destroy();
	delete myScene;
	return 0;
}