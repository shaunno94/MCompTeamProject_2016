#include <cstdio>
#include "Rendering\Renderer.h"
#include "PhysicsEngine\PhysicsEngineInstance.h"
#include "Rendering\ModelLoader.h"
#include "Rendering\DebugDraw.h"
#include "Rendering\GameTimer.h"
#include "Rendering\LocalControlManager.h"
#include "Stadium.h"

const float TIME_STEP = 1.0f / 120.0f;
const unsigned int SUB_STEPS = 4;

#ifndef ORBIS
#include "Rendering\KeyboardController.h"
const unsigned int SCREEN_HEIGHT = 800;
const unsigned int SCREEN_WIDTH = 1280;
const string SIMPLESHADER_VERT = SHADER_DIR"textureVertex.glsl";
const string SIMPLESHADER_FRAG = SHADER_DIR"textureFragment.glsl";
const string POINTLIGHTSHADER_VERT = SHADER_DIR"2dShadowLightvertex.glsl";
const string POINTLIGHTSHADER_FRAG = SHADER_DIR"2dShadowLightfragment.glsl";
#else
#include "Input\PS4Input.h"
#include "Rendering\PS4Controller.h"
const unsigned int SCREEN_HEIGHT = 1080;
const unsigned int SCREEN_WIDTH = 1920;
const string SIMPLESHADER_VERT = SHADER_DIR"textureVertex.sb";
const string SIMPLESHADER_FRAG = SHADER_DIR"textureFragment.sb";
const string POINTLIGHTSHADER_VERT = SHADER_DIR"2dShadowLightvertex.sb";
const string POINTLIGHTSHADER_FRAG = SHADER_DIR"2dShadowLightfragment.sb";
//System Variables
unsigned int sceLibcHeapExtendedAlloc = 1;			/* Switch to dynamic allocation */
size_t sceLibcHeapSize = 512 * 1024 * 1024;			/* Set up heap area upper limit as 256 MiB */
//int sceUserMainThreadPriority = SCE_KERNEL_DEFAULT_PRIORITY_USER;
#endif

struct GoalBallCollisionFilter : public btOverlapFilterCallback
{

public:

	int m_ballID = 0;
	int m_goal1ID = 0;
	int m_goal2ID = 0;

	virtual bool needBroadphaseCollision(btBroadphaseProxy* proxy0, btBroadphaseProxy* proxy1) const override
	{
		if ((proxy0->getUid() == m_ballID && proxy1->getUid() == m_goal1ID) ||
		    (proxy1->getUid() == m_ballID && proxy0->getUid() == m_goal1ID))
		{
			//TODO: Increment goals for team 1
			int ifojwe = 8;

			//TODO: Reset Scene
		}
		else if((proxy0->getUid() == m_ballID && proxy1->getUid() == m_goal2ID) ||
			(proxy1->getUid() == m_ballID && proxy0->getUid() == m_goal2ID))
		{
			//TODO: Increment goals for team 2
			int ifojwe = 8;

			//TODO: Reset Scene
		}
		return true;
	}
};

int main(void)
{
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
	myScene->getCamera()->SetPosition(Vec3Graphics(10, 5, 0)); //no effect anymore

	ControllerManager* myControllers = new LocalControlManager;

	//Game objects added to scene are delete by the scene so don't delete twice.
	GameObject* player = new GameObject("player");
	GameObject* ball = new GameObject("ball");
	GameObject* light1 = new GameObject("l");
	GameObject* light2 = new GameObject("l");

	GameObject* ai1 = new GameObject("ai1");
	GameObject* ai2 = new GameObject("ai2");
	GameObject* ai3 = new GameObject("ai3");

	//Physics objects hold collision shape and collision object(body),
	//call CreateCollisionShape before CreatePhysicsBody or the object will not be created correctly.
	//Physics objects will be deleted by the game object.
	RigidPhysicsObject* playerPhysics = new RigidPhysicsObject();
	playerPhysics->CreateCollisionShape(Vec3Physics(5.0, 2.5, 5.0),CUBOID);
	playerPhysics->CreatePhysicsBody(8.0, Vec3Physics(10, 5, 0), QuatPhysics(0, 0, 0, 1), Vec3Physics(1, 1, 1));



	RigidPhysicsObject* ai1Physics = new RigidPhysicsObject();
	ai1Physics->CreateCollisionShape(Vec3Physics(5.0, 2.5, 5.0), CUBOID);
	ai1Physics->CreatePhysicsBody(8.0, Vec3Physics(-150, 5, 50), QuatPhysics(0, 0, 0, 1), Vec3Physics(1, 1, 1));

	RigidPhysicsObject* ai2Physics = new RigidPhysicsObject();
	ai2Physics->CreateCollisionShape(Vec3Physics(5.0, 2.5, 5.0), CUBOID);
	ai2Physics->CreatePhysicsBody(8.0, Vec3Physics(-250, 5, 0), QuatPhysics(0, 0, 0, 1), Vec3Physics(1, 1, 1));

	RigidPhysicsObject* ai3Physics = new RigidPhysicsObject();
	ai3Physics->CreateCollisionShape(Vec3Physics(5.0, 2.5, 5.0), CUBOID);
	ai3Physics->CreatePhysicsBody(8.0, Vec3Physics(-200, 5, -50), QuatPhysics(0, 0, 0, 1), Vec3Physics(1, 1, 1));



	RigidPhysicsObject* ballPhysics = new RigidPhysicsObject();
	ballPhysics->CreateCollisionShape(7.0);
	ballPhysics->CreatePhysicsBody(2.0, Vec3Physics(0, 3, 0), QuatPhysics(0, 0, 0, 1), Vec3Physics(1, 1, 1));
	int ballID = ballPhysics->GetPhysicsBody()->getBroadphaseProxy()->getUid();

	RigidPhysicsObject* floorPhysics = new RigidPhysicsObject();
	floorPhysics->CreateCollisionShape(0, Vec3Physics(0, 1, 0), true);
	floorPhysics->CreatePhysicsBody(0, Vec3Physics(0, -1, 0), QuatPhysics(0, 0, 0, 1));

#ifndef ORBIS
	BaseShader* simpleShader = new OGLShader(SIMPLESHADER_VERT, SIMPLESHADER_FRAG);
	BaseShader* pointlightShader = new OGLShader(POINTLIGHTSHADER_VERT, POINTLIGHTSHADER_FRAG);
	//BaseShader* pointlightShader = new OGLShader(SHADER_DIR"CubeShadowLightvertex.glsl", SHADER_DIR"CubeShadowLightfragment.glsl");
#else
	BaseShader* simpleShader = new PS4Shader(SIMPLESHADER_VERT, SIMPLESHADER_FRAG);
	BaseShader* pointlightShader = new PS4Shader(POINTLIGHTSHADER_VERT, POINTLIGHTSHADER_FRAG);
#endif

	if (!pointlightShader->IsOperational() || !simpleShader->IsOperational())
		return -1;

	LightMaterial* lightMaterial = new LightMaterial(pointlightShader);
	light1->SetRenderComponent(new RenderComponent(lightMaterial, ModelLoader::LoadMGL(MODEL_DIR"Common/ico.mgl", true)));
	light1->SetWorldTransform(Mat4Graphics::Translation(Vec3Graphics(0, 2, 0.3f)) *Mat4Graphics::Scale(Vec3Graphics(20, 20, 20)));
	light1->SetBoundingRadius(20);

	lightMaterial->shadowType = _2D;
	light2->SetRenderComponent(new RenderComponent(lightMaterial, ModelLoader::LoadMGL(MODEL_DIR"Common/ico.mgl", true)));
	light2->SetWorldTransform(Mat4Graphics::Translation(Vec3Graphics(400, 600, 400)) *Mat4Graphics::Scale(Vec3Graphics(1600, 1600, 1600)));
	light2->SetBoundingRadius(1600);

	Material* material = new Material(simpleShader);
	Material* ballMaterial = new Material(simpleShader);
	Material* netMaterial = new Material(simpleShader);
	netMaterial->hasTranslucency = true;
	ballMaterial->Set(ReservedMeshTextures.DIFFUSE.name, Texture::Get(TEXTURE_DIR"checkerboard.tga", true));

	renderer.SetCurrentScene(myScene);

	// Create Stadium
	GameObject* stadium = new Stadium(material, netMaterial, "stadium");

	myScene->addGameObject(stadium);
	//myScene->addLightObject(light1);
	myScene->addLightObject(light2);

	player->SetRenderComponent(new RenderComponent(ballMaterial, ModelLoader::LoadMGL(MODEL_DIR"Common/cube.mgl", true)));
	player->SetLocalTransform(Mat4Graphics::Scale(Vector3Simple(5, 2.5f, 5)));
	player->SetPhysicsComponent(playerPhysics);
	player->GetPhysicsComponent()->GetPhysicsBody()->setRestitution(btScalar(0.9));
	player->GetPhysicsComponent()->GetPhysicsBody()->setFriction(0.5);
	player->GetPhysicsComponent()->GetPhysicsBody()->setRollingFriction(0.5);
	player->GetPhysicsComponent()->GetPhysicsBody()->setHitFraction(0.5);

	ball->SetRenderComponent(new RenderComponent(ballMaterial, ModelLoader::LoadMGL(MODEL_DIR"Common/sphere.mgl", true)));
	ball->SetLocalTransform(Mat4Graphics::Scale(Vector3Simple(7, 7, 7)));
	ball->SetPhysicsComponent(ballPhysics);
	ball->GetPhysicsComponent()->GetPhysicsBody()->setRestitution(btScalar(0.9));
	ball->GetPhysicsComponent()->GetPhysicsBody()->setFriction(0.5);
	ball->GetPhysicsComponent()->GetPhysicsBody()->setRollingFriction(0.5);
	ball->GetPhysicsComponent()->GetPhysicsBody()->setHitFraction(0.5);

	ai1->SetRenderComponent(new RenderComponent(ballMaterial, ModelLoader::LoadMGL(MODEL_DIR"Common/cube.mgl", true)));
	ai1->SetLocalTransform(Mat4Graphics::Scale(Vector3Simple(5, 2.5f, 5)));
	ai1->SetPhysicsComponent(ai1Physics);
	ai1->GetPhysicsComponent()->GetPhysicsBody()->setRestitution(btScalar(0.9));
	ai1->GetPhysicsComponent()->GetPhysicsBody()->setFriction(0.5);
	ai1->GetPhysicsComponent()->GetPhysicsBody()->setRollingFriction(0.5);
	ai1->GetPhysicsComponent()->GetPhysicsBody()->setHitFraction(0.5);

	ai2->SetRenderComponent(new RenderComponent(ballMaterial, ModelLoader::LoadMGL(MODEL_DIR"Common/cube.mgl", true)));
	ai2->SetLocalTransform(Mat4Graphics::Scale(Vector3Simple(5, 2.5f, 5)));
	ai2->SetPhysicsComponent(ai2Physics);
	ai2->GetPhysicsComponent()->GetPhysicsBody()->setRestitution(btScalar(0.9));
	ai2->GetPhysicsComponent()->GetPhysicsBody()->setFriction(0.5);
	ai2->GetPhysicsComponent()->GetPhysicsBody()->setRollingFriction(0.5);
	ai2->GetPhysicsComponent()->GetPhysicsBody()->setHitFraction(0.5);

	ai3->SetRenderComponent(new RenderComponent(ballMaterial, ModelLoader::LoadMGL(MODEL_DIR"Common/cube.mgl", true)));
	ai3->SetLocalTransform(Mat4Graphics::Scale(Vector3Simple(5, 2.5f, 5)));
	ai3->SetPhysicsComponent(ai3Physics);
	ai3->GetPhysicsComponent()->GetPhysicsBody()->setRestitution(btScalar(0.9));
	ai3->GetPhysicsComponent()->GetPhysicsBody()->setFriction(0.5);
	ai3->GetPhysicsComponent()->GetPhysicsBody()->setRollingFriction(0.5);
	ai3->GetPhysicsComponent()->GetPhysicsBody()->setHitFraction(0.5);

	RigidPhysicsObject* goalBox = new RigidPhysicsObject();
	goalBox->CreateCollisionShape(Vec3Physics(7.0, 15.0, 29.0), CUBOID);
	goalBox->CreatePhysicsBody(0.0, Vec3Physics(268, 17, 0), QuatPhysics(0, 0, 0, 1), Vec3Physics(1, 1, 1), true);
	int goal1ID = goalBox->GetPhysicsBody()->getBroadphaseProxy()->getUid();

	RigidPhysicsObject* goalBox2 = new RigidPhysicsObject();
	goalBox2->CreateCollisionShape(Vec3Physics(7.0, 15.0, 29.0), CUBOID);
	goalBox2->CreatePhysicsBody(0.0, Vec3Physics(-268, 17, 0), QuatPhysics(0, 0, 0, 1), Vec3Physics(1, 1, 1), true);
	int goal2ID = goalBox2->GetPhysicsBody()->getBroadphaseProxy()->getUid();

	GameObject* goal1 = new GameObject("goal1");
	goal1->SetPhysicsComponent(goalBox);

	GameObject* goal2 = new GameObject("goal2");
	goal2->SetPhysicsComponent(goalBox2);


	GoalBallCollisionFilter filter;
	filter.m_ballID = ballID;
	filter.m_goal1ID = goal1ID;
	filter.m_goal2ID = goal2ID;

	PhysicsEngineInstance::Instance()->getPairCache()->setOverlapFilterCallback(&filter);

	myScene->addGameObject(goal1);

	ControllerComponent* cc = new ControllerComponent(player);
#ifndef ORBIS
	myControllers->setProducer(player);
#else
	myScene->setPlayerController(new PS4Controller(cc));
#endif

	myScene->attachCam(player);

	myScene->addGameObject(player);
	myScene->addGameObject(ball);
	myScene->addGameObject(ai1);
	//myScene->addGameObject(ai2);
	//myScene->addGameObject(ai3);


	myControllers->setActor(ai1, 0);
	//myControllers->setActor(ai2, 0);
	//myControllers->setActor(ai3, 0);




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