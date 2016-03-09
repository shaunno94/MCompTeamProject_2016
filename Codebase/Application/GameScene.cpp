#include "GameScene.h"
#include "BallGameObject.h"

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
	SoundSystem::Release();
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
	myControllers->setActor(Renderer::GetInstance()->GetCurrentScene()->findGameObject("shooterAI"), SHOOTER);
	myControllers->setActor(Renderer::GetInstance()->GetCurrentScene()->findGameObject("goalieAI"), GOALKEEPER);
	myControllers->setActor(Renderer::GetInstance()->GetCurrentScene()->findGameObject("aggroAI"), AGGRESSIVE);
}

void GameScene::IncrementScore(int team)
{
	scores[team % 2]++;

	SoundMOD mod;
	mod.looping = false;
	mod.isGlobal = true;
	SoundSystem::Instance()->Play(SoundManager::GetSound(BANG), mod);
	scoreboardComponent->Update(scores[0], scores[1], currentTime);
}

void GameScene::UpdateScene(float dt)
{
	if (currentTime > 180)
	{
		//TODO: Proceed to end game screen
		currentTime = 0;
		lastTime = 0;
	}
	currentTime += dt / 1000.0f;

	if (currentTime < 181)
	{
		lastTime = currentTime;
		scoreboardComponent->Update(scores[0], scores[1], currentTime);
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

	shooterAI = new CarGameObject(Vec3Physics(-190, 2, 30), QuatPhysics(0, 0, 0, 1), aiMaterial, "shooterAI", COL_AI_CAR);

	goalieAI = new CarGameObject(Vec3Physics(-230, 2, 0), QuatPhysics(0, 0, 0, 1), aiMaterial, "goalieAI", COL_AI_CAR);

	aggroAI = new CarGameObject(Vec3Physics(-190, 2, -30), QuatPhysics(0, 0, 0, 1), ai2Material, "aggroAI", COL_AI_CAR);


	// Create Stadium
	stadium = new Stadium(material, netMaterial, "stadium");

	goal1 = new GameObject("goal1");
	goalBox = new RigidPhysicsObject();
	goalBox->CreateCollisionShape(Vec3Physics(7.0, 15.0, 35.0) * 1.5f, CUBOID);
	goalBox->CreatePhysicsBody(0.0, Vec3Physics(268, 17, 0) * 1.5f, QuatPhysics(0, 0, 0, 1), Vec3Physics(1, 1, 1), true);
	goal1->SetPhysicsComponent(goalBox);

	goal2 = new GameObject("goal2");
	goalBox2 = new RigidPhysicsObject();
	goalBox2->CreateCollisionShape(Vec3Physics(7.0, 15.0, 35.0) * 1.5f, CUBOID);
	goalBox2->CreatePhysicsBody(0.0, Vec3Physics(-268, 17, 0) * 1.5f, QuatPhysics(0, 0, 0, 1), Vec3Physics(1, 1, 1), true);
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

	goalBallFilter = new GameCollisionFilter(this);
	PhysicsEngineInstance::Instance()->getPairCache()->setOverlapFilterCallback(goalBallFilter);
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
	aggroAI->SetAudioComponent(new AudioCompCar(false));
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
	netMaterial = new Material(simpleShader, true);
	aiMaterial = new Material(simpleShader);
	particleMaterial = new Material(simpleShader);
	ai2Material = new Material(simpleShader);
	guiMaterial = new Material(orthoShader);
	textMaterial = new Material(orthoShader);

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
	scoreboardComponent = new ScoreboardGUIComponent(guiMaterial, Texture::Get(TEXTURE_DIR"tahoma.tga"), 1.0);
	hudOrtho->AddGUIComponent(scoreboardComponent);

	//Add Orthographic component to GUISystem
	GUISystem::GetInstance().AddOrthoComponent(hudOrtho);

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

	attenuation = max(1 - (attenuation / (600 * 600)), 0.0f);

	dynamic_cast<RigidPhysicsObject*>(car->GetPhysicsComponent())->GetPhysicsBody()->applyCentralImpulse(ballToCar * attenuation * 30000000.0f);
}

void GameScene::SetupAI()
{
	dynamic_cast<LocalControlManager*>(myControllers)->setupActors();
}

void GameScene::ResetObjects()
{
	btVector3 zeroVector = btVector3(0, 0, 0);
	PhysicsEngineInstance::Instance()->clearForces();

	dynamic_cast<RigidPhysicsObject*>(ball->GetPhysicsComponent())->GetPhysicsBody()->clearForces();
	dynamic_cast<RigidPhysicsObject*>(ball->GetPhysicsComponent())->GetPhysicsBody()->setLinearVelocity(zeroVector);
	dynamic_cast<RigidPhysicsObject*>(ball->GetPhysicsComponent())->GetPhysicsBody()->setAngularVelocity(zeroVector);

	ball->GetPhysicsComponent()->GetPhysicsBody()->setWorldTransform(btTransform(btQuaternion(0, 0, 0, 1), zeroVector));

	//TODO: Reset player positions
}


GameCollisionFilter::GameCollisionFilter(GameScene* scene) : m_scene(scene) {
	static_cast<RigidPhysicsObject*>(scene->findGameObject("ball")->GetPhysicsComponent())->GetPhysicsBody()->getBroadphaseProxy()->m_collisionFilterMask = COL_BALL;

	m_ballID = static_cast<RigidPhysicsObject*>(scene->findGameObject("ball")->GetPhysicsComponent())->GetPhysicsBody()->getBroadphaseProxy()->getUid();
	m_goal1ID = static_cast<RigidPhysicsObject*>(scene->findGameObject("goal1")->GetPhysicsComponent())->GetPhysicsBody()->getBroadphaseProxy()->getUid();
	m_goal2ID = static_cast<RigidPhysicsObject*>(scene->findGameObject("goal2")->GetPhysicsComponent())->GetPhysicsBody()->getBroadphaseProxy()->getUid();
}

bool GameCollisionFilter::needBroadphaseCollision(btBroadphaseProxy* proxy0, btBroadphaseProxy* proxy1) const
{
	short int combined = COL_CAR | COL_WALL;
	/*int combinedMask = proxy0->m_collisionFilterMask | proxy1->m_collisionFilterMask;
	int test1 = (combinedMask & COL_CAR) == COL_CAR;
	int test2 = (combinedMask & COL_WALL) == COL_WALL;
	int test3;*/
	if ((proxy0->m_collisionFilterMask | proxy1->m_collisionFilterMask) & combined == combined)
	{
		int test = 0;
	}
	else
	{
		int test2 = 0;
	}
	//if (((combinedMask & COL_CAR) == COL_CAR) && ((combinedMask & COL_WALL) == COL_WALL)) {
	//	std::cout << "Car and wall collision" << std::endl;
	//	// sort out car-wall collision
	//}

	if ((proxy0->getUid() == m_ballID && proxy1->getUid() == m_goal1ID) ||
		(proxy1->getUid() == m_ballID && proxy0->getUid() == m_goal1ID))
	{
		m_scene->SetGoalScored(1);
	}
	else if ((proxy0->getUid() == m_ballID && proxy1->getUid() == m_goal2ID) ||
		(proxy1->getUid() == m_ballID && proxy0->getUid() == m_goal2ID))
	{
		m_scene->SetGoalScored(2);
	}
	return true;
}