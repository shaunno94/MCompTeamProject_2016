#include "GameScene.h"
#include "BallGameObject.h"
#include "GoalCollisionFilterStep.h"
#include "PickupManager.h"
#include "AI/constants.h"

GameScene::GameScene()
{
	//initialise conroller manager
	myControllers = new LocalControlManager();

	//Initialise Bullet physics engine.
	PhysicsEngineInstance::Instance()->setGravity(btVector3(0, -9.81, 0));

#ifndef ORBIS
	ParticleManager::Initialise();

	if (ParticleManager::GetManager().HasInitialised())
	{
		std::cout << "Particle Manager not Initialised" << std::endl;
	}
#endif
	guiSystem = new GUISystem();
	if (!guiSystem->HasInitialised())
	{
		std::cout << "GUI not Initialised!" << std::endl;
	}
	scores[0] = 0;
	scores[1] = 0;

#if DEBUG_DRAW
	PhysicsEngineInstance::Instance()->setDebugDrawer(DebugDraw::Instance());
	DebugDraw::Context(Renderer::GetInstance());
#endif

	SetupShaders();
	SetupMaterials();
	SetupGameObjects();
	DrawGUI();
	SetupControls();
}


GameScene::~GameScene()
{
	PhysicsEngineInstance::Release();

#ifndef ORBIS
	ParticleManager::Destroy();
#endif

	if (guiSystem)
		delete guiSystem;

	//delete pickupManager;

#if DEBUG_DRAW
	DebugDraw::Release();
#endif
}

void GameScene::SetControllerActor()
{
	myControllers->setActor(Renderer::GetInstance()->GetCurrentScene()->findGameObject("shooterAI"), SHOOTER);
	myControllers->setActor(Renderer::GetInstance()->GetCurrentScene()->findGameObject("goalieAI"), GOALKEEPER);
	//myControllers->setActor(Renderer::GetInstance()->GetCurrentScene()->findGameObject("aggroAI"), AGGRESSIVE);
	myControllers->setActor(Renderer::GetInstance()->GetCurrentScene()->findGameObject("aggroAI"), SHOOTER); // Whilst debugging
}

void GameScene::IncrementScore(int team)
{
	scores[team % 2]++;

#ifndef ORBIS
	SoundMOD mod;
	mod.looping = false;
	mod.isGlobal = true;
	SoundSystem::Instance()->Play(SoundManager::GetSound(BANG), mod);
#endif

	scoreboardComponent->Update(scores[0], scores[1], currentTime);
}

void GameScene::UpdateScene(float dt)
{
	myControllers->update(dt);

	if (currentTime > 180)
	{
		currentTime = 0;
		lastTime = 0;
	}
	currentTime += dt / 1000.0f;

	if (currentTime - lastTime > 1)
	{
		lastTime = currentTime;
		scoreboardComponent->Update(scores[0], scores[1], currentTime);
		//GET_DEBUG_STREAM().str().substr()

#ifdef _DEBUG
		
		vector<std::string>tokens;
		std::string t;
		while (std::getline(GET_DEBUG_STREAM(), t, '\n')) {
			tokens.push_back(t.substr(2, std::string::npos));
		}

		float fpsStep = 1/(std::stof(tokens[1]) + std::stof(tokens[3])) * 1000;
		float physicsStep = std::stof(tokens[1]);
		std::string fps = "FPS: " + std::to_string(fpsStep).substr(0,5);
		std::string physics = "Physics: " + tokens[1].substr(0, 5);
		std::string graphics = "Graphics: " + tokens[3].substr(0, 5);

		FPSDebugTextComponent->Update(fps);
		physicsDebugTextComponent->Update(physics);
		graphicsDebugTextComponent->Update(graphics);
#endif
	}

	if (goalScored > 0) {
		if (timerCount == 0)
		{
			IncrementScore(goalScored - 1);
			TriggerExplosion();
		}

		timerCount += dt / 1000.0f;

		//Increment goals for team 1
		if (timerCount > 3.0f)
		{
			timerCount = 0;
			goalScored = false;
			ResetObjects();
		}
	}

}

void GameScene::SetupGameObjects()
{
	light1 = new GameObject("l");
	light1->SetRenderComponent(new RenderComponent(lightMaterial, ModelLoader::LoadMGL(MODEL_DIR"Common/ico.mgl", true)));
	light1->SetWorldTransform(Mat4Graphics::Translation(Vec3Graphics(0, 2, 0.3f)) *Mat4Graphics::Scale(Vec3Graphics(20, 20, 20)));
	light1->SetBoundingRadius(20);

	light2 = new GameObject("l");
	light2->SetRenderComponent(new RenderComponent(lightMaterial, ModelLoader::LoadMGL(MODEL_DIR"Common/ico.mgl", true)));
	light2->SetWorldTransform(Mat4Graphics::Translation(Vec3Graphics(600, 900, 600)) *Mat4Graphics::Scale(Vec3Graphics(2400, 2400, 2400)));
	light2->SetBoundingRadius(2400);

	auto ballMaterial = new Material(simpleShader);
	ballMaterial->Set(ReservedMeshTextures.DIFFUSE.name, Texture::Get(TEXTURE_DIR"football.png", true));
	ball = new BallGameObject("ball", ballMaterial);

	player = new CarGameObject(Vec3Physics(100, 2, 0), QuatPhysics(0, 1, 0, 1), playerMaterial, "player");

	shooterAI = new CarGameObject(Vec3Physics(-190, 2, 30), QuatPhysics::IDENTITY, aiMaterial, "shooterAI", COL_AI_CAR);

	goalieAI = new CarGameObject(Vec3Physics(-230, 2, 0), QuatPhysics::IDENTITY, aiMaterial, "goalieAI", COL_AI_CAR);

	pickupManager = new PickupManager(material);
	for (auto pickupMapping : *(pickupManager->GetPickups()))
	{
		addGameObject(pickupMapping.second);
	}
	aggroAI = new CarGameObject(Vec3Physics(-190, 2, -30), QuatPhysics(0, 0, 0, 1), ai2Material, "aggroAI", COL_AI_CAR);


	// Create Stadium
	stadium = new Stadium(material, netMaterial, postMaterial, "stadium");

	goal1 = new GameObject("goal1");
	goalBox = new RigidPhysicsObject();
	goalBox->CreateCollisionShape(Vec3Physics(7.0, 15.0, 35.0) * 1.5f, CUBOID);
	goalBox->CreatePhysicsBody(0.0, Vec3Physics(268, 17, 0) * 1.5f, QuatPhysics::IDENTITY, Vec3Physics::ONES, true);
	goalBox->GetPhysicsBody()->getBroadphaseProxy()->m_collisionFilterGroup = COL_GROUP_DEFAULT;
	goalBox->GetPhysicsBody()->getBroadphaseProxy()->m_collisionFilterMask = COL_GOAL1;
	goal1->SetPhysicsComponent(goalBox);

	goal2 = new GameObject("goal2");
	goalBox2 = new RigidPhysicsObject();
	goalBox2->CreateCollisionShape(Vec3Physics(7.0, 15.0, 35.0) * 1.5f, CUBOID);
	goalBox2->CreatePhysicsBody(0.0, Vec3Physics(-268, 17, 0) * 1.5f, QuatPhysics::IDENTITY, Vec3Physics::ONES, true);
	goalBox2->GetPhysicsBody()->getBroadphaseProxy()->m_collisionFilterGroup = COL_GROUP_DEFAULT;
	goalBox2->GetPhysicsBody()->getBroadphaseProxy()->m_collisionFilterMask = COL_GOAL2;
	goal2->SetPhysicsComponent(goalBox2);


	addGameObject(stadium);
	addGameObject(player);
	addGameObject(ball);
	addGameObject(shooterAI);
	addGameObject(goalieAI);
	addGameObject(aggroAI);
	addGameObject(goal1);
	addGameObject(goal2);

	addLightObject(light2);

	PhysicsEngineInstance::GetFilter().steps.push_back(new GoalCollisionFilterStep());
}

void GameScene::LoadAudio()
{
//#ifndef ORBIS
	//-------- SOUND
	// load in files
	SoundManager::LoadAssets();

	//load ogg file (EXPERIMENTAL STREAMING)
	//SoundManager::AddSound(AUDIO_DIR"139320__votives__arpegthing.ogg", "song");

	// add background music
	SoundSystem::Instance()->SetBackgroundMusic(SoundManager::GetSound(SONG));
	SoundSystem::Instance()->SetBackgroundVolume(0.4f); // can be used for mute / unmute

	// create audio components
	player->SetAudioComponent(new AudioCompCarLitener(true));
	shooterAI->SetAudioComponent(new AudioCompCar(false));
	goalieAI->SetAudioComponent(new AudioCompCar(false));
	aggroAI->SetAudioComponent(new AudioCompCar(false));
	//-------- SOUND
//#endif
}

void GameScene::SetupShaders()
{
#ifndef ORBIS
	simpleShader = new OGLShader(SIMPLESHADER_VERT, SIMPLESHADER_FRAG);
	pointlightShader = new OGLShader(POINTLIGHTSHADER_VERT, POINTLIGHTSHADER_FRAG);
	orthoShader = new OGLShader(GUI_VERT, GUI_FRAG);
#else
	simpleShader = new PS4Shader(SIMPLESHADER_VERT, SIMPLESHADER_FRAG);
	pointlightShader = new PS4Shader(POINTLIGHTSHADER_VERT, POINTLIGHTSHADER_FRAG);
	orthoShader = new PS4Shader(GUI_VERT, GUI_FRAG);
#endif
	if (!pointlightShader->IsOperational())
		std::cout << "Point light shader not operational!" << std::endl;
	if(!simpleShader->IsOperational())
		std::cout << "Simple shader not operational!" << std::endl;
	if(!orthoShader->IsOperational())
		std::cout << "ortho shader not operational!" << std::endl;
}

void GameScene::SetupMaterials()
{
	lightMaterial = new LightMaterial(pointlightShader);
	lightMaterial->shadowType = _2D;

	material = new Material(simpleShader);
	netMaterial = new Material(simpleShader, true);
	postMaterial = new Material(simpleShader, true);
	aiMaterial = new Material(simpleShader);
	particleMaterial = new Material(simpleShader);
	ai2Material = new Material(simpleShader);
	guiMaterial = new Material(orthoShader);
	textMaterial = new Material(orthoShader);
	playerMaterial = new Material(simpleShader);

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

	scoreboardComponent = new ScoreboardGUIComponent(guiMaterial, std::to_string(0) + " - " + "3:00" + " - 0", Vec3Graphics(-0.6f, 0.7f, 0), Vec3Graphics(0.1f, 0.1f, 1));
	hudOrtho->AddGUIComponent(scoreboardComponent);

#if _DEBUG
	FPSDebugTextComponent = new TextGUIComponent(guiMaterial, GET_DEBUG_STREAM().str(), Vec3Graphics(-1.0f, -0.7f, 0), Vec3Graphics(0.04f, 0.04f, 1));
	hudOrtho->AddGUIComponent(FPSDebugTextComponent);
	physicsDebugTextComponent = new TextGUIComponent(guiMaterial, GET_DEBUG_STREAM().str(), Vec3Graphics(-1.0f, -0.8f, 0), Vec3Graphics(0.04f, 0.04f, 1));
	hudOrtho->AddGUIComponent(physicsDebugTextComponent);
	graphicsDebugTextComponent = new TextGUIComponent(guiMaterial, GET_DEBUG_STREAM().str(), Vec3Graphics(-1.0f, -0.9f, 0), Vec3Graphics(0.04f, 0.04f, 1));
	hudOrtho->AddGUIComponent(graphicsDebugTextComponent);
#endif

	//Add Orthographic component to GUISystem
	guiSystem->AddOrthoComponent(hudOrtho);
}

void GameScene::SetupControls()
{
	cc = new ControllerComponent(player);
	myControllers->setProducer(player, 0);

	attachCam(player);
}

void GameScene::TriggerExplosion()
{
	// Do other things before resetting scene fully

	applyImpulseFromExplosion((CarGameObject*)player);
	applyImpulseFromExplosion((CarGameObject*)shooterAI);
	applyImpulseFromExplosion((CarGameObject*)goalieAI);

}

void GameScene::applyImpulseFromExplosion(CarGameObject* car)
{
	btVector3 ballPos = ball->GetPhysicsComponent()->GetPhysicsBody()->getWorldTransform().getOrigin();

	btVector3 carPos = car->GetPhysicsComponent()->GetPhysicsBody()->getWorldTransform().getOrigin();

	btVector3 ballToCar = carPos - ballPos;

	float attenuation = ballToCar.length2();
	ballToCar.normalize();

	// 1 at same position
	// 0 at 200 units away

	attenuation = fmax(1 - (attenuation / (600 * 600)), 0.0f);

	dynamic_cast<RigidPhysicsObject*>(car->GetPhysicsComponent())->GetPhysicsBody()->applyCentralImpulse(ballToCar * attenuation * 30000000.0f);
}

void GameScene::SetupAI()
{
	dynamic_cast<LocalControlManager*>(myControllers)->setupActors();
}

void GameScene::ResetObjects()
//reset the positions and forces of objects in the scene
{
	PhysicsEngineInstance::Instance()->clearForces();

	//TODO: Reset player positions
	ResetObject(*ball);
	ResetObject(*player);
	ResetObject(*shooterAI);
	ResetObject(*goalieAI);
	ResetObject(*aggroAI);

	player->SetWorldTransform(Mat4Graphics::RotationY(-90) * Mat4Graphics::Translation(Vec3Graphics(100, 2, 0)));//have to reset this world transform too, for the camera
	cam->reset();
}

void GameScene::ResetObject(GameObject& object) {

	btVector3 zeroVector = btVector3(0, 0, 0);
	dynamic_cast<RigidPhysicsObject*>(object.GetPhysicsComponent())->GetPhysicsBody()->clearForces();
	dynamic_cast<RigidPhysicsObject*>(object.GetPhysicsComponent())->GetPhysicsBody()->setLinearVelocity(zeroVector);
	dynamic_cast<RigidPhysicsObject*>(object.GetPhysicsComponent())->GetPhysicsBody()->setAngularVelocity(zeroVector);

	object.GetPhysicsComponent()->GetPhysicsBody()->setWorldTransform(btTransform(btQuaternion(0, 0, 0, 1), btVector3(object.GetSpawnPoint().x, object.GetSpawnPoint().y, object.GetSpawnPoint().z)));
	if (object.GetControllerComponent()) 
		object.GetControllerComponent()->reset();

}
void GameScene::Setup()
{
	SetControllerActor();
	SetupAI();
	LoadAudio();
}

void GameScene::Cleanup()
{

}