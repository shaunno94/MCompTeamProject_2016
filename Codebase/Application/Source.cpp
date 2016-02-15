#include <cstdio>
#include "Rendering\Window.h"
#include "Rendering\Renderer.h"
#include "PhysicsEngine\PhysicsEngineInstance.h"
#include "Rendering/ModelLoader.h"
#include "Rendering/LightMaterial.h"

const float TIME_STEP = 1.0f / 60.0f;
const unsigned int SUB_STEPS = 10;

int main()
{
	//-------------------
	//--- MAIN ENGINE ---
	//-------------------

	//Initialise the Window
	if (!Window::Initialise("Game Technologies - Framework Example", 1280, 800, false))
	{
		Window::Destroy();
		return -1;
	}

	Window::GetWindow().LockMouseToWindow(true);
	Window::GetWindow().ShowOSPointer(false);

	/*CommonMeshes::InitializeMeshes();
	NCLDebug::LoadShaders();*/

	Renderer renderer(Window::GetWindow());
	if (!renderer.HasInitialised())
	{
		return -1;
	}
	
	//Initialise Bullet physics engine.
	PhysicsEngineInstance::Instance()->setGravity(btVector3(0, -9.81, 0));

	//Test Scenario - Tardis (cuboid collision shape), floor (cuboid collision shape), ball (sphere collison shape)
	Scene* myScene = new Scene();
	//Game objects added to scene are delete by the scene so don't delete twice.
	GameObject* ball = new GameObject("ball");
	GameObject* floor = new GameObject("floor");
	GameObject* light = new GameObject("l");

	Shader* simpleShader = new Shader(SHADER_DIR"textureVertex.glsl", SHADER_DIR"textureFragment.glsl");
	Shader* pointlightShader = new Shader(SHADER_DIR"pointlightvertex.glsl", SHADER_DIR"pointlightfragment.glsl");

	if (!pointlightShader->IsOperational())
		return -1;

	if (!simpleShader->IsOperational())
		return -1;

	LightMaterial* lightMaterial = new LightMaterial(pointlightShader);
	light->SetRenderComponent(new RenderComponent(lightMaterial, ModelLoader::LoadMGL(MODEL_DIR"Common/ico.mgl", true)));
	light->SetWorldTransform(Mat4Graphics::Translation(Vec3Graphics(0, 2, 2)) *Mat4Graphics::Scale(Vec3Graphics(20,20,20)));
	light->SetBoundingRadius(20);


	Material* material = new Material(simpleShader);
	floor->SetRenderComponent(new RenderComponent(material, Mesh::GenerateQuad()));
	test->SetRenderComponent(new RenderComponent(material, ModelLoader::LoadMGL(MODEL_DIR"Common/ico.mgl", true)));
	//test->InitPhysics();
	floor->SetWorldTransform(Mat4Graphics::Translation(Vec3Graphics(0, -2, 0)) *Mat4Graphics::Rotation(90, Vec3Graphics(1, 0, 0)) * Mat4Graphics::Scale(Vec3Graphics(450, 450, 1)));
	auto tex = Texture::Get(TEXTURE_DIR"brick.tga");
	//floor->GetRenderComponent()->m_Material->Set("diffuseTex", tex);
	floor->GetRenderComponent()->m_Mesh->SetTexture(tex, ReservedMeshTextures.DIFFUSE.index);
	tex->Clear();

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
	myScene->addGameObject(floor);
	myScene->addLightObject(light);

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