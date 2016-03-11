#include "GameScene.h"
#include "BallGameObject.h"
#include "GoalCollisionFilterStep.h"
#include "PickupManager.h"
#include "AI/constants.h"
#include "constants.h"
#include <algorithm>  


GameScene::GameScene()
{

	//initialise conroller manager
	myControllers = new LocalControlManager();

	guiSystem = new GUISystem();
	if (!guiSystem->HasInitialised())
	{
		std::cout << "GUI not Initialised!" << std::endl;
	}

	SetupShaders();
	SetupMaterials();
	DrawGUI();
}


GameScene::~GameScene()
{

	delete material;
	delete netMaterial;
	delete redPostMaterial;
	delete bluePostMaterial;
	delete ballMaterial;

	delete playerMaterial;
	delete aiMaterial;
	delete ai2Material;
	delete particleMaterial;
	delete guiMaterial;
	delete textMaterial;

	delete pickupManager;

	delete myControllers;
	myControllers = nullptr;

		delete guiSystem;
	guiSystem = nullptr;


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

	if (currentTime > 10)
	{
		Scene* endScene = Renderer::GetInstance()->GetScene(END_SCENE);
		team winner;
		if (scores[0] > scores[1])
			((EndScene*)endScene)->SetWinningTeam(BLUE_TEAM);
		else if(scores[0] < scores[1])
			((EndScene*)endScene)->SetWinningTeam(RED_TEAM);
		else
			((EndScene*)endScene)->SetWinningTeam(TIE);
		Renderer::GetInstance()->SetCurrentScene(endScene);
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

	float boost = floor(std::max(player->GetControllerComponent()->boost * 100.0f, 0.0f));
	if (boost > 99.0f) {
		boostComponent->Update(std::string("Boost:" + std::to_string(boost).substr(0, 3) + "%"));
	}
	else if (boost < 10.0f) {
		boostComponent->Update(std::string("Boost:" + std::to_string(boost).substr(0, 1) + "%"));
	} 
	else {
		boostComponent->Update(std::string("Boost:" + std::to_string(boost).substr(0, 2) + "%"));
	}

	float speed = floor(std::max(player->GetControllerComponent()->getForwardVelocity(), 0.0f)) / 2.0f;
	if (speed > 99.0f) {
		speedComponent->Update(std::string(std::to_string(speed).substr(0,3) + "mph"));
	}
	else if (speed < 10.0f) {
		speedComponent->Update(std::string(std::to_string(speed).substr(0, 1) + "mph"));
	}
	else {
		speedComponent->Update(std::string(std::to_string(speed).substr(0, 2) + "mph"));
	}
}

void GameScene::SetupGameObjects()
{
	light1 = new GameObject("light1");
	light1->SetRenderComponent(new RenderComponent(lightMaterial, ModelLoader::LoadMGL(MODEL_DIR"Common/ico.mgl", true)));
	light1->SetWorldTransform(Mat4Graphics::Translation(Vec3Graphics(0, 2, 0.3f)) *Mat4Graphics::Scale(Vec3Graphics(20, 20, 20)));
	light1->SetBoundingRadius(20);

	light2 = new GameObject("light2");
	light2->SetRenderComponent(new RenderComponent(lightMaterial, ModelLoader::LoadMGL(MODEL_DIR"Common/ico.mgl", true)));
	light2->SetWorldTransform(Mat4Graphics::Translation(Vec3Graphics(600, 900, 600)) *Mat4Graphics::Scale(Vec3Graphics(2400, 2400, 2400)));
	light2->SetBoundingRadius(2400);

	ball = new BallGameObject("ball", ballMaterial);

	player = new CarGameObject(Vec3Physics(195, 2, 0), QuatPhysics(0, 1, 0, 1), playerMaterial, RED_TEAM, "player");
	shooterAI = new CarGameObject(Vec3Physics(-190, 2, 30), QuatPhysics::IDENTITY, aiMaterial, BLUE_TEAM, "shooterAI", COL_AI_CAR);
	goalieAI = new CarGameObject(Vec3Physics(-330, 2, 0), QuatPhysics::IDENTITY, aiMaterial, BLUE_TEAM, "goalieAI", COL_AI_CAR);
	aggroAI = new CarGameObject(Vec3Physics(-190, 2, -30), QuatPhysics(0, 0, 0, 1), ai2Material, BLUE_TEAM, "aggroAI", COL_AI_CAR);

	pickupManager = new PickupManager(material);
	for (auto pickupMapping : *(pickupManager->GetPickups()))
	{
		addGameObject(pickupMapping.second);
	}


	// Create Stadium
	stadium = new Stadium(material, netMaterial, redPostMaterial, bluePostMaterial, "stadium");

	redGoal = new GameObject("redGoal");
	redGoalBox = new RigidPhysicsObject();
	redGoalBox->CreateCollisionShape(Vec3Physics(7.0, 15.0, 35.0) * 1.5f, CUBOID);
	redGoalBox->CreatePhysicsBody(0.0, Vec3Physics(268, 17, 0) * 1.5f, QuatPhysics::IDENTITY, Vec3Physics::ONES, true);
	redGoalBox->GetPhysicsBody()->getBroadphaseProxy()->m_collisionFilterGroup = COL_GROUP_DEFAULT;
	redGoalBox->GetPhysicsBody()->getBroadphaseProxy()->m_collisionFilterMask = COL_GOAL1;
	redGoal->SetPhysicsComponent(redGoalBox);

	blueGoal = new GameObject("blueGoal");
	blueGoalBox = new RigidPhysicsObject();
	blueGoalBox->CreateCollisionShape(Vec3Physics(7.0, 15.0, 35.0) * 1.5f, CUBOID);
	blueGoalBox->CreatePhysicsBody(0.0, Vec3Physics(-268, 17, 0) * 1.5f, QuatPhysics::IDENTITY, Vec3Physics::ONES, true);
	blueGoalBox->GetPhysicsBody()->getBroadphaseProxy()->m_collisionFilterGroup = COL_GROUP_DEFAULT;
	blueGoalBox->GetPhysicsBody()->getBroadphaseProxy()->m_collisionFilterMask = COL_GOAL2;
	blueGoal->SetPhysicsComponent(blueGoalBox);


	addGameObject(stadium);
	addGameObject(player);
	addGameObject(ball);
	addGameObject(shooterAI);
	addGameObject(goalieAI);
	addGameObject(aggroAI);
	addGameObject(redGoal);
	addGameObject(blueGoal);

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
	colourShader = new OGLShader(SIMPLESHADER_VERT, COLOURSHADER_FRAG);
	pointlightShader = new OGLShader(POINTLIGHTSHADER_VERT, POINTLIGHTSHADER_FRAG);
	orthoShader = new OGLShader(GUI_VERT, GUI_FRAG);
#else
	simpleShader = new PS4Shader(SIMPLESHADER_VERT, SIMPLESHADER_FRAG);
	colourShader = new PS4Shader(SIMPLESHADER_VERT, SIMPLESHADER_FRAG);
	pointlightShader = new PS4Shader(POINTLIGHTSHADER_VERT, POINTLIGHTSHADER_FRAG);
	orthoShader = new PS4Shader(GUI_VERT, GUI_FRAG);
#endif
	if (!pointlightShader->IsOperational())
		std::cout << "Point light shader not operational!" << std::endl;
	if(!simpleShader->IsOperational())
		std::cout << "Simple shader not operational!" << std::endl;
	if (!colourShader->IsOperational())
		std::cout << "Colour shader not operational!" << std::endl;
	if(!orthoShader->IsOperational())
		std::cout << "ortho shader not operational!" << std::endl;
}

void GameScene::SetupMaterials()
{
	lightMaterial = new LightMaterial(pointlightShader);
	lightMaterial->shadowType = _2D;

	material = new Material(simpleShader);
	netMaterial = new Material(simpleShader, true);
	ballMaterial = new Material(simpleShader);
	redPostMaterial = new ExtendedMaterial(colourShader, true);
	bluePostMaterial = new ExtendedMaterial(colourShader, true);
	aiMaterial = new Material(simpleShader);
	particleMaterial = new Material(simpleShader);
	ai2Material = new Material(simpleShader);
	guiMaterial = new Material(orthoShader);
	textMaterial = new Material(orthoShader);
	playerMaterial = new Material(simpleShader);

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

	speedComponent = new TextGUIComponent(guiMaterial, "0mph", Vec3Graphics(0.53f, -0.8f, 0), Vec3Graphics(0.05f, 0.05f, 1));
	hudOrtho->AddGUIComponent(speedComponent);

	boostComponent = new TextGUIComponent(guiMaterial, "Boost:0%", Vec3Graphics(0.45f, -0.9f, 0), Vec3Graphics(0.05f, 0.05f, 1));
	hudOrtho->AddGUIComponent(boostComponent);
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

	//Reset player positions
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
	Scene::Setup();
	scores[0] = 0;
	scores[1] = 0;

	SetupGameObjects();
	SetupControls();
	SetControllerActor();
	SetupAI();
	LoadAudio();

}

void GameScene::Cleanup()
{
	/*ResetObjects();
	scores[0] = scores[1] = 0;*/
	Scene::Cleanup();
	ClearObjects();

}