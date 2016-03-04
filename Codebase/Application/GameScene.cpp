#include "GameScene.h"

GameScene::GameScene(ControllerManager* controller)
: myControllers(controller)
{
	//Initialise Bullet physics engine.
	PhysicsEngineInstance::Instance()->setGravity(btVector3(0, -9.81, 0));

	SoundSystem::Initialise();
	GUISystem::Initialise();
	ParticleManager::Initialise();

	if (ParticleManager::GetManager().HasInitialised())
	{
		std::cout << "Particle Manager not Initialised" << std::endl;
	}

	if (!GUISystem::GetInstance().HasInitialised())
	{
		std::cout << "GUI not Initialised!" << std::endl;
	}
	scores[0] = 0;
	scores[1] = 0;

#if DEBUG_DRAW
#ifndef ORBIS
	PhysicsEngineInstance::Instance()->setDebugDrawer(DebugDraw::Instance());
	DebugDraw::Context(Renderer::GetInstance());
#endif
#endif

	SetupShaders();
	SetupMaterials();
	SetupGameObjects();
	DrawGUI();
	LoadAudio();
	SetupControls();
}


GameScene::~GameScene()
{
	PhysicsEngineInstance::Release();
	GUISystem::Destroy();
	ParticleManager::Destroy();

#if DEBUG_DRAW
#ifndef ORBIS
	DebugDraw::Release();
#endif
#endif
}

void GameScene::SetControllerActor()
{
	myControllers->setActor(Renderer::GetInstance()->GetCurrentScene()->findGameObject("shooterAI"), 0);
	myControllers->setActor(Renderer::GetInstance()->GetCurrentScene()->findGameObject("goalieAI"), 1);
}

void GameScene::SetupGameObjects()
{
	light1 = new GameObject("l");
	light1->SetRenderComponent(new RenderComponent(lightMaterial, ModelLoader::LoadMGL(MODEL_DIR"Common/ico.mgl", true)));
	light1->SetWorldTransform(Mat4Graphics::Translation(Vec3Graphics(0, 2, 0.3f)) *Mat4Graphics::Scale(Vec3Graphics(20, 20, 20)));
	light1->SetBoundingRadius(20);

	light2 = new GameObject("l");
	light2->SetRenderComponent(new RenderComponent(lightMaterial, ModelLoader::LoadMGL(MODEL_DIR"Common/ico.mgl", true)));
	light2->SetWorldTransform(Mat4Graphics::Translation(Vec3Graphics(400, 600, 400)) *Mat4Graphics::Scale(Vec3Graphics(1600, 1600, 1600)));
	light2->SetBoundingRadius(1600);

	ball = new GameObject("ball");
	ballPhysics = new RigidPhysicsObject();
	ballPhysics->CreateCollisionShape(7.0);
	ballPhysics->CreatePhysicsBody(1.0, Vec3Physics(0, 3, 0), QuatPhysics(0, 0, 0, 1), Vec3Physics(1, 1, 1));
	ballID = ballPhysics->GetPhysicsBody()->getBroadphaseProxy()->getUid();

	ball->SetRenderComponent(new RenderComponent(ballMaterial, ModelLoader::LoadMGL(MODEL_DIR"Common/sphere.mgl", true)));
	ball->SetLocalTransform(Mat4Graphics::Scale(Vector3Simple(7, 7, 7)));


	ballPhysics->GetPhysicsBody()->getBroadphaseProxy()->m_collisionFilterMask = COL_BALL;

	ball->SetPhysicsComponent(ballPhysics);
	ball->GetPhysicsComponent()->GetPhysicsBody()->setRestitution(btScalar(1.6));
	ball->GetPhysicsComponent()->GetPhysicsBody()->setFriction(0.5);
	ball->GetPhysicsComponent()->GetPhysicsBody()->setRollingFriction(0.5);
	ball->GetPhysicsComponent()->GetPhysicsBody()->setHitFraction(0.5);
	player = new CarGameObject(Vec3Physics(100, 5, 0), QuatPhysics(0, 1, 0, 1), playerMaterial, "player");

	shooterAI = new CarGameObject(Vec3Physics(-190, 5, 30), QuatPhysics(0, 0, 0, 1), aiMaterial, "shooterAI", COL_AI_CAR);

	goalieAI = new CarGameObject(Vec3Physics(-230, 5, -30), QuatPhysics(0, 0, 0, 1), ai2Material, "goalieAI", COL_AI_CAR);


	// Create Stadium
	stadium = new Stadium(material, netMaterial, "stadium");

	goal1 = new GameObject("goal1");
	goalBox = new RigidPhysicsObject();
	goalBox->CreateCollisionShape(Vec3Physics(7.0, 15.0, 29.0), CUBOID);
	goalBox->CreatePhysicsBody(0.0, Vec3Physics(268, 17, 0), QuatPhysics(0, 0, 0, 1), Vec3Physics(1, 1, 1), true);
	goal1ID = goalBox->GetPhysicsBody()->getBroadphaseProxy()->getUid();
	goal1->SetPhysicsComponent(goalBox);

	goal2 = new GameObject("goal2");
	goalBox2 = new RigidPhysicsObject();
	goalBox2->CreateCollisionShape(Vec3Physics(7.0, 15.0, 29.0), CUBOID);
	goalBox2->CreatePhysicsBody(0.0, Vec3Physics(-268, 17, 0), QuatPhysics(0, 0, 0, 1), Vec3Physics(1, 1, 1), true);
	goal2ID = goalBox2->GetPhysicsBody()->getBroadphaseProxy()->getUid();
	goal2->SetPhysicsComponent(goalBox2);

	goalBallFilter = new GameCollisionFilter(this);
	goalBallFilter->m_ballID = ballID;
	goalBallFilter->m_goal1ID = goal1ID;
	goalBallFilter->m_goal2ID = goal2ID;

	PhysicsEngineInstance::Instance()->getPairCache()->setOverlapFilterCallback(goalBallFilter);
	addGameObject(stadium);
	addGameObject(player);
	addGameObject(ball);
	addGameObject(shooterAI);
	addGameObject(goalieAI);
	addGameObject(goal1);
	addGameObject(goal2);

	addLightObject(light2);
}

void GameScene::LoadAudio()
{
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
	goalieAI->SetAudioComponent(new AudioCompCar(false));
	//-------- SOUND
}

void GameScene::SetupShaders()
{
#ifndef ORBIS
	simpleShader = new OGLShader(SIMPLESHADER_VERT, SIMPLESHADER_FRAG);
	pointlightShader = new OGLShader(POINTLIGHTSHADER_VERT, POINTLIGHTSHADER_FRAG);
	orthoShader = new OGLShader(GUI_VERT, GUI_FRAG);
	//BaseShader* pointlightShader = new OGLShader(SHADER_DIR"CubeShadowLightvertex.glsl", SHADER_DIR"CubeShadowLightfragment.glsl");
#else
	BaseShader* simpleShader = new PS4Shader(SIMPLESHADER_VERT, SIMPLESHADER_FRAG);
	BaseShader* pointlightShader = new PS4Shader(POINTLIGHTSHADER_VERT, POINTLIGHTSHADER_FRAG);
#endif

	if (!pointlightShader->IsOperational() || !simpleShader->IsOperational() || !orthoShader->IsOperational())
		std::cout << "Shader not operational!" << std::endl;
}

void GameScene::SetupMaterials()
{
	lightMaterial = new LightMaterial(pointlightShader);
	lightMaterial->shadowType = _2D;

	material = new Material(simpleShader);
	ballMaterial = new Material(simpleShader);
	netMaterial = new Material(simpleShader, true);
	aiMaterial = new Material(simpleShader);
	particleMaterial = new Material(simpleShader);
	ai2Material = new Material(simpleShader);
	guiMaterial = new Material(orthoShader);
	textMaterial = new Material(orthoShader);

	ballMaterial->Set(ReservedMeshTextures.DIFFUSE.name, Texture::Get(TEXTURE_DIR"football.png", true));
	playerMaterial = new Material(simpleShader);

	aiMaterial->Set(ReservedMeshTextures.DIFFUSE.name, Texture::Get(MODEL_DIR"car/body1.bmp", true));

	ai2Material->Set(ReservedMeshTextures.DIFFUSE.name, Texture::Get(MODEL_DIR"car/body2.bmp", true));

	//particleMaterial->Set(ReservedMeshTextures.DIFFUSE.name, Texture::Get(TEXTURE_DIR"particle.tga", true));
}

void GameScene::DrawGUI()
{

	//Define Orthographic Component
	hudOrtho = new OrthoComponent(1.0f);
	//Add child GUI components, while defining materials, texture, and depth
	scoreboardComponent = new ScoreboardGUIComponent(guiMaterial, Texture::Get(TEXTURE_DIR"tahoma.tga"), 1.0, true, "scoreboard");
	hudOrtho->AddGUIComponent(scoreboardComponent);

	//Add Orthographic component to GUISystem
	GUISystem::GetInstance().AddOrthoComponent(hudOrtho);

}

void GameScene::SetupControls()
{
	cc = new ControllerComponent(player);
	myControllers->setProducer(player);

	attachCam(player);
}

void GameScene::ResetScene()
{
	// Do other things before resetting scene fully
	ResetObjects();
}

void GameScene::ResetObjects()
{
	btVector3 zeroVector = btVector3(0, 0, 0);

	dynamic_cast<RigidPhysicsObject*>(ball->GetPhysicsComponent())->GetPhysicsBody()->clearForces();
	dynamic_cast<RigidPhysicsObject*>(ball->GetPhysicsComponent())->GetPhysicsBody()->setLinearVelocity(zeroVector);
	dynamic_cast<RigidPhysicsObject*>(ball->GetPhysicsComponent())->GetPhysicsBody()->setAngularVelocity(zeroVector);

	ball->GetPhysicsComponent()->GetPhysicsBody()->setWorldTransform(btTransform(btQuaternion(0, 0, 0, 1), zeroVector));

}
