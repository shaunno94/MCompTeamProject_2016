#ifndef ORBIS
#include "NetClientGameScene.h"
#include "BallGameObject.h"
#include "GoalCollisionFilterStep.h"
#include "constants.h"
#include "Rendering/NetClientControllerManager.h"

NetClientGameScene::NetClientGameScene()
{
}


NetClientGameScene::~NetClientGameScene()
{

}

void NetClientGameScene::Setup()
{
	///GameScene::Setup();
	Scene::Setup();
	scores[0] = 0;
	scores[1] = 0;

	light2 = new GameObject("light2");
	light2->SetRenderComponent(new RenderComponent(lightMaterial, ModelLoader::LoadMGL(MODEL_DIR"Common/ico.mgl", true)));
	light2->SetWorldTransform(Mat4Graphics::Translation(Vec3Graphics(600, 900, 600)) *Mat4Graphics::Scale(Vec3Graphics(2400, 2400, 2400)));
	light2->SetBoundingRadius(2400);

	for (int i = 0; i < NetworkGameData::posCount; ++i)
	{
		m_cars.push_back(new CarGameObject(NetworkGameData::Instance.pos[i], NetworkGameData::Instance.quat[i], playerMaterial, RED_TEAM, i == NetworkGameData::Instance.client->GetSessionMemberId() ? "player" : "enemy"));

		addGameObject(m_cars[i]);
	}

	ball = new BallGameObject("ball", ballMaterial);
	addGameObject(ball);

	player = m_cars[NetworkGameData::Instance.client->GetSessionMemberId()];
	//shooterAI = new CarGameObject(Vec3Physics(-190, 2, 30), QuatPhysics::IDENTITY, aiMaterial, BLUE_TEAM, "shooterAI", COL_AI_CAR);
	//goalieAI = new CarGameObject(Vec3Physics(-330, 2, 0), QuatPhysics::IDENTITY, aiMaterial, BLUE_TEAM, "goalieAI", COL_AI_CAR);
	//aggroAI = new CarGameObject(Vec3Physics(-190, 2, -30), QuatPhysics(0, 0, 0, 1), ai2Material, BLUE_TEAM, "aggroAI", COL_AI_CAR);

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
	addGameObject(ball);
	//addGameObject(player);
	//addGameObject(shooterAI);
	//addGameObject(goalieAI);
	//addGameObject(aggroAI);
	addGameObject(redGoal);
	addGameObject(blueGoal);

	addLightObject(light2);

	PhysicsEngineInstance::GetFilter().steps.push_back(new GoalCollisionFilterStep());

	while (!NetworkGameData::Instance.client->GetSession()->GetState() == NetSessionInactive) //waiting for everyone to send the ready message
	{}

	if (myControllers)
	{
		delete myControllers;
	}
	myControllers = new NetClientControllerManager(NetworkGameData::Instance.client->GetSession());

	//SetupGameObjects();
	//SetupControls();
	for (int i = 0; i < NetworkGameData::posCount; ++i)
	{
		if (i == NetworkGameData::Instance.server->GetSessionMemberId())
		{
			cc = new ControllerComponent(m_cars[i]);
			myControllers->setProducer(m_cars[i], i);
			attachCam(m_cars[i]);
		}
		else
		{
			cc = new ControllerComponent(m_cars[i]);
			myControllers->setActor(m_cars[i], i);
		}
	}
	myControllers->setActor(ball, NetworkGameData::posCount);



	//SetControllerActor();
	//SetupAI();

	//LoadAudio();

	SoundManager::LoadAssets();

	//load ogg file (EXPERIMENTAL STREAMING)
	//SoundManager::AddSound(AUDIO_DIR"139320__votives__arpegthing.ogg", "song");

	// add background music
	SoundSystem::Instance()->SetBackgroundMusic(SoundManager::GetSound(SONG));
	SoundSystem::Instance()->SetBackgroundVolume(0.4f); // can be used for mute / unmute

	// create audio components
	player->SetAudioComponent(new AudioCompCarLitener(true));
	//shooterAI->SetAudioComponent(new AudioCompCar(false));
	//goalieAI->SetAudioComponent(new AudioCompCar(false));
	//aggroAI->SetAudioComponent(new AudioCompCar(false));

	for (int i = 0; i < NetworkGameData::posCount; ++i)
	{
		//m_cars[i] = new CarGameObject(NetworkGameData::Instance.pos[i], NetworkGameData::Instance.quat[i], playerMaterial, RED_TEAM, "player");
		if (i == NetworkGameData::Instance.client->GetSessionMemberId())
		{
			m_cars[i]->SetAudioComponent(new AudioCompCarLitener(true));
		}
		else
		{
			m_cars[i]->SetAudioComponent(new AudioCompCar(false));
		}
	}
}

void NetClientGameScene::Cleanup()
{
	GameScene::Cleanup();
	//Scene::Cleanup();
	//ClearObjects();
	//delete NetworkGameData::Instance.client;
	//NetworkGameData::Instance.client = nullptr;
}

#endif