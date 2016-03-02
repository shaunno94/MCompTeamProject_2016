#include <cstdio>
#include "Rendering\Renderer.h"
#include "PhysicsEngine\PhysicsEngineInstance.h"
#include "Rendering\ModelLoader.h"
#include "Rendering\DebugDraw.h"
#include "Rendering\GameTimer.h"
#include "Rendering\LocalControlManager.h"
#include "Stadium.h"
#include "CarGameObject.h"
#include "Rendering\GUISystem.h"
#include "Rendering\ScoreboardGUIComponent.h"
#include "Audio\SoundSystem.h"

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
const string GUI_VERT = SHADER_DIR"combineVert.glsl";
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

	bool CheckIfGoal(btBroadphaseProxy* proxy0, btBroadphaseProxy* proxy1) const
	{
		if ((proxy0->getUid() == m_ballID && proxy1->getUid() == m_goal1ID) ||
			(proxy1->getUid() == m_ballID && proxy0->getUid() == m_goal1ID))
		{
			//TODO: Increment goals for team 1
			int ifojwe = 8;
			//TODO: Reset Scene
		}
		else if ((proxy0->getUid() == m_ballID && proxy1->getUid() == m_goal2ID) ||
			(proxy1->getUid() == m_ballID && proxy0->getUid() == m_goal2ID))
		{
			//TODO: Increment goals for team 2
			int ifojwe = 8;
			//TODO: Reset Scene
		}
		return true;
	}

	virtual bool needBroadphaseCollision(btBroadphaseProxy* proxy0, btBroadphaseProxy* proxy1) const override
	{
		if (proxy0->m_collisionFilterMask == COL_GOAL
			|| proxy1->m_collisionFilterMask == COL_GOAL)
		{
			return CheckIfGoal(proxy0, proxy1);
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

	GUISystem::Initialise();	
	if (!GUISystem::GetInstance().HasInitialised())
	{
		return -1;
	}

	//Initialise Bullet physics engine.
	PhysicsEngineInstance::Instance()->setGravity(btVector3(0, -9.81, 0));

	//Init SoundSystem
	SoundSystem::Initialise();

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
	GameObject* ball = new GameObject("ball");
	GameObject* light1 = new GameObject("l");
	GameObject* light2 = new GameObject("l");


	//Physics objects hold collision shape and collision object(body), 
	//call CreateCollisionShape before CreatePhysicsBody or the object will not be created correctly.
	//Physics objects will be deleted by the game object.

	RigidPhysicsObject* ballPhysics = new RigidPhysicsObject();
	ballPhysics->CreateCollisionShape(7.0);
	ballPhysics->CreatePhysicsBody(1.0, Vec3Physics(0, 3, 0), QuatPhysics(0, 0, 0, 1), Vec3Physics(1, 1, 1));
	ballPhysics->GetPhysicsBody()->getBroadphaseProxy()->m_collisionFilterMask = COL_BALL;
	ballPhysics->GetPhysicsBody()->getBroadphaseProxy()->m_collisionFilterGroup = GROUP_WALL_BALL;
#
	ballPhysics->GetPhysicsBody()->setRestitution(btScalar(0.9));
	ballPhysics->GetPhysicsBody()->setFriction(0.5);
	ballPhysics->GetPhysicsBody()->setRollingFriction(0.5);
	ballPhysics->GetPhysicsBody()->setHitFraction(0.5);
	int ballID = ballPhysics->GetPhysicsBody()->getBroadphaseProxy()->getUid();
	
#ifndef ORBIS
	BaseShader* simpleShader = new OGLShader(SIMPLESHADER_VERT, SIMPLESHADER_FRAG);
	BaseShader* pointlightShader = new OGLShader(POINTLIGHTSHADER_VERT, POINTLIGHTSHADER_FRAG);
	BaseShader* orthoShader = new OGLShader(GUI_VERT, SIMPLESHADER_FRAG);
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
	Material* netMaterial = new Material(simpleShader, true);
	//Material* guiMaterial = new Material(orthoShader);

	ballMaterial->Set(ReservedMeshTextures.DIFFUSE.name, Texture::Get(TEXTURE_DIR"checkerboard.tga", true));
	Material* playerMaterial = new Material(simpleShader);

	Material* aiMaterial = new Material(simpleShader);
	aiMaterial->Set(ReservedMeshTextures.DIFFUSE.name, Texture::Get(MODEL_DIR"car/body1.bmp", true));

	Material* ai2Material = new Material(simpleShader);
	ai2Material->Set(ReservedMeshTextures.DIFFUSE.name, Texture::Get(MODEL_DIR"car/body2.bmp", true));

	renderer.SetCurrentScene(myScene);

	GameObject* player = new CarGameObject(Vec3Physics(100, 5, 0), QuatPhysics(0, 1, 0, 1), playerMaterial, "player");
	GameObject* shooterAI = new CarGameObject(Vec3Physics(-190, 5, 30), QuatPhysics(0, -1, 0, 1), aiMaterial, "shooterAI");
	GameObject* goalieAI = new CarGameObject(Vec3Physics(-230, 5, -30), QuatPhysics(0, -1, 0, 1), ai2Material, "goalieAI");


	// Create Stadium
	GameObject* stadium = new Stadium(material, netMaterial, "stadium"); 

	myScene->addGameObject(stadium);
	//myScene->addLightObject(light1);
	myScene->addLightObject(light2);

	ball->SetRenderComponent(new RenderComponent(ballMaterial, ModelLoader::LoadMGL(MODEL_DIR"Common/sphere.mgl", true)));
	ball->SetLocalTransform(Mat4Graphics::Scale(Vector3Simple(7, 7, 7)));
	ball->SetPhysicsComponent(ballPhysics);

	RigidPhysicsObject* goalBox = new RigidPhysicsObject();
	goalBox->CreateCollisionShape(Vec3Physics(7.0, 15.0, 29.0), CUBOID);
	goalBox->CreatePhysicsBody(0.0, Vec3Physics(268, 17, 0), QuatPhysics(0, 0, 0, 1), Vec3Physics(1, 1, 1), true);
	int goal1ID = goalBox->GetPhysicsBody()->getBroadphaseProxy()->getUid();
	goalBox->GetPhysicsBody()->getBroadphaseProxy()->m_collisionFilterMask = COL_GOAL;

	RigidPhysicsObject* goalBox2 = new RigidPhysicsObject();
	goalBox2->CreateCollisionShape(Vec3Physics(7.0, 15.0, 29.0), CUBOID);
	goalBox2->CreatePhysicsBody(0.0, Vec3Physics(-268, 17, 0), QuatPhysics(0, 0, 0, 1), Vec3Physics(1, 1, 1), true);
	int goal2ID = goalBox2->GetPhysicsBody()->getBroadphaseProxy()->getUid();
	goalBox2->GetPhysicsBody()->getBroadphaseProxy()->m_collisionFilterMask = COL_GOAL;

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
	myScene->addGameObject(goal2);

	ControllerComponent* cc = new ControllerComponent(player);
#ifndef ORBIS
	myControllers->setProducer(player);
#else
	myScene->setPlayerController(new PS4Controller(cc));
#endif

	////Define Orthographic Component
	//OrthoComponent* hudUI = new OrthoComponent(1.0f);
	////Add child GUI components, while defining materials, texture, and depth
	//hudUI->AddGUIComponent(new ScoreboardGUIComponent(guiMaterial, Texture::Get(TEXTURE_DIR"blue3.png"), 1.0));

	////Add Orthographic component to GUISystem
	//GUISystem::GetInstance().AddOrthoComponent(hudUI);

	myScene->attachCam(player);
	myScene->addGameObject(player);
	myScene->addGameObject(ball);
	myScene->addGameObject(shooterAI);
	myScene->addGameObject(goalieAI);

	//-------- SOUND
	// load in files
	SoundManager::LoadAssets();

	//load ogg file (EXPERIMENTAL STREAMING)
	//SoundManager::AddSound(AUDIO_DIR"139320__votives__arpegthing.ogg", "song");

	// add background music
	SoundSystem::Instance()->SetBackgroundMusic(SoundManager::GetSound(SONG));
	SoundSystem::Instance()->SetBackgroundVolume(0.4f); // can be used for mute / unmute

	//modify sound values (for later)
	SoundMOD mod = SoundMOD();
	mod.isGlobal = true;
	mod.looping = false;
	mod.volume = 0.45f;

	// create audio components
	player->SetAudioComponent(new AudioCompCarLitener(true));
	shooterAI->SetAudioComponent(new AudioCompCar(false));
	//-------- SOUND

	renderer.SetCurrentScene(myScene);

	myControllers->setActor(shooterAI, 0);
	myControllers->setActor(goalieAI, 1);


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

		//-------- SOUND
		// just play a sound
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_K)){
			mod.pitch = 1.75f;

			SoundSystem::Instance()->Play(SoundManager::GetSound(GUN), mod);
		}

		SoundSystem::Instance()->Update(ms);
		//-------- SOUND

	}

	//Cleanup
	PhysicsEngineInstance::Release();
	//Destroys all GUI on the scene
	GUISystem::Destroy();
	SoundSystem::Release();

#if DEBUG_DRAW
#ifndef ORBIS
	DebugDraw::Release();
#endif
#endif

	delete myScene;
	return 0;
}