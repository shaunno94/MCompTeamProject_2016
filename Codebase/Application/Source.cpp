#include <cstdio>
#include "Rendering\Renderer.h"
#include "PhysicsEngine\PhysicsEngineInstance.h"
#include "Rendering\ModelLoader.h"
#include "Rendering\DebugDraw.h"
#include "Rendering\GameTimer.h"
#include "Stadium.h"

// Includes for AI States and Triggers
#include "AI\StateMachine.h"
#include "AI\ChaseState.h"
#include "AI\RunAwayState.h"
#include "AI\DistanceTrigger.h"
#include "AI\ShooterAgent.h"
#include "Rendering\KeyboardController.h"

const float TIME_STEP = 1.0f / 120.0f;
const unsigned int SUB_STEPS = 4;

#ifndef ORBIS
const unsigned int SCREEN_HEIGHT = 800;
const unsigned int SCREEN_WIDTH = 1280;
const string SIMPLESHADER_VERT = SHADER_DIR"textureVertex.glsl";
const string SIMPLESHADER_FRAG = SHADER_DIR"textureFragment.glsl";
const string POINTLIGHTSHADER_VERT = SHADER_DIR"2dShadowLightvertex.glsl";
const string POINTLIGHTSHADER_FRAG = SHADER_DIR"2dShadowLightfragment.glsl";
#else
const unsigned int SCREEN_HEIGHT = 1080;
const unsigned int SCREEN_WIDTH = 1920;
const string SIMPLESHADER_VERT = SHADER_DIR"textureVertex.sb";
const string SIMPLESHADER_FRAG = SHADER_DIR"textureFragment.sb";
const string POINTLIGHTSHADER_VERT = SHADER_DIR"2dShadowLightvertex.sb";
const string POINTLIGHTSHADER_FRAG = SHADER_DIR"2dShadowLightfragment.sb";
//System Variables
unsigned int sceLibcHeapExtendedAlloc = 1;			/* Switch to dynamic allocation */
size_t sceLibcHeapSize = 512 * 1024 * 1024;			/* Set up heap area upper limit as 512 MiB */
//int sceUserMainThreadPriority = SCE_KERNEL_DEFAULT_PRIORITY_USER;
#endif

int main(void) {
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

	//Initialise Bullet physics engine.
	PhysicsEngineInstance::Instance()->setGravity(btVector3(0, -9.81, 0));

#if DEBUG_DRAW
#ifndef ORBIS
	PhysicsEngineInstance::Instance()->setDebugDrawer(DebugDraw::Instance());
	DebugDraw::Context(&renderer);
#endif
#endif

	//Test Scenario - Tardis (cuboid collision shape), floor (plane collision shape), ball (sphere collison shape)
	Scene* myScene = new Scene();
	myScene->getCamera()->SetPosition(Vec3Graphics(0, 0, 0));

	//Game objects added to scene are delete by the scene so don't delete twice.
	GameObject* ball = new GameObject("ball");
	GameObject* light1 = new GameObject("l");
	GameObject* light2 = new GameObject("l");

	//Physics objects hold collision shape and collision object(body), 
	//call CreateCollisionShape before CreatePhysicsBody or the object will not be created correctly.
	//Physics objects will be deleted by the game object.
	RigidPhysicsObject* ballPhysics = new RigidPhysicsObject();
	ballPhysics->CreateCollisionShape(Vec3Physics(5.0, 5.0, 5.0),CUBOID);
	ballPhysics->CreatePhysicsBody(5.0, Vec3Physics(0, 5, 0), QuatPhysics(0, 0, 0, 1), Vec3Physics(1, 1, 1));

	RigidPhysicsObject* floorPhysics = new RigidPhysicsObject();
	floorPhysics->CreateCollisionShape(0, Vec3Physics(0, 1, 0), true);
	floorPhysics->CreatePhysicsBody(0, Vec3Physics(0, -1, 0), QuatPhysics(0, 0, 0, 1));

#ifndef ORBIS
	BaseShader* simpleShader = new OGLShader(SIMPLESHADER_VERT, SIMPLESHADER_FRAG);
	BaseShader* pointlightShader = new OGLShader(POINTLIGHTSHADER_VERT, POINTLIGHTSHADER_FRAG);
#else
	BaseShader* simpleShader = new PS4Shader(SIMPLESHADER_VERT, SIMPLESHADER_FRAG);
	//BaseShader* pointlightShader = new PS4Shader(POINTLIGHTSHADER_VERT, POINTLIGHTSHADER_FRAG);
#endif
	
	if (/*!pointlightShader->IsOperational() ||*/ !simpleShader->IsOperational())
		return -1;

	/*LightMaterial* lightMaterial = new LightMaterial(pointlightShader);
	light1->SetRenderComponent(new RenderComponent(lightMaterial, ModelLoader::LoadMGL(MODEL_DIR"Common/ico.mgl", true)));
	light1->SetWorldTransform(Mat4Graphics::Translation(Vec3Graphics(0, 2, 2)) *Mat4Graphics::Scale(Vec3Graphics(20, 20, 20)));
	light1->SetBoundingRadius(20);

	lightMaterial->shadowType = _2D;
	light2->SetRenderComponent(new RenderComponent(lightMaterial, ModelLoader::LoadMGL(MODEL_DIR"Common/ico.mgl", true)));
	light2->SetWorldTransform(Mat4Graphics::Translation(Vec3Graphics(600, 600, 600)) *Mat4Graphics::Scale(Vec3Graphics(1600, 1600, 1600)));
	light2->SetBoundingRadius(1600);*/

	/*Material* ballMaterial = new Material(simpleShader);
	ballMaterial->Set(ReservedMeshTextures.DIFFUSE.name, Texture::Get(TEXTURE_DIR"checkerboard.tga", true));*/
	Material* material = new Material(simpleShader);
	Material* ballMaterial = new Material(simpleShader);
	Material* netMaterial = new Material(simpleShader);
	netMaterial->hasTranslucency = true;
	ballMaterial->Set(ReservedMeshTextures.DIFFUSE.name, Texture::Get(TEXTURE_DIR"checkerboard.tga", true));

	// Create Stadium
	GameObject* stadium = new Stadium(material, netMaterial, "stadium"); 

	GameObject* test = new GameObject("test");
	test->SetRenderComponent(new RenderComponent(material, Mesh::GenerateQuad()));
	myScene->addGameObject(test);

	ball->SetRenderComponent(new RenderComponent(ballMaterial, ModelLoader::LoadMGL(MODEL_DIR"Common/cube.mgl", true)));
	ball->SetLocalTransform(Mat4Graphics::Scale(Vector3Simple(5, 5, 5)));
	ball->SetPhysicsComponent(ballPhysics);
	ball->GetPhysicsComponent()->GetPhysicsBody()->setRestitution(btScalar(0.9));
	ball->GetPhysicsComponent()->GetPhysicsBody()->setFriction(0.5);
	ball->GetPhysicsComponent()->GetPhysicsBody()->setRollingFriction(0.5);
	ball->GetPhysicsComponent()->GetPhysicsBody()->setHitFraction(0.5);

	ControllerComponent* cc = new ControllerComponent(ball);
	myScene->setPlayerController(new KeyboardController(cc));

	myScene->attachCam(ball);


	myScene->addGameObject(ball);

	renderer.SetCurrentScene(myScene);


#ifndef ORBIS
	while (Window::GetWindow().UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE))
#else
	while (true)
#endif
	{
		float ms = timer.GetTimer()->Get(1000.0f);
		PhysicsEngineInstance::Instance()->stepSimulation(ms, SUB_STEPS, TIME_STEP);
		renderer.RenderScene(ms);
	}

	//Cleanup
	PhysicsEngineInstance::Release();

#if DEBUG_DRAW
#ifndef ORBIS
	DebugDraw::Release();
#endif
#endif

	delete myScene;
	return 0;
}