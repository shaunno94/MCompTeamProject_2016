#include "GameScene.h"


GameScene::GameScene(ControllerManager* controller)
	: myControllers(controller)
{

	//Initialise Bullet physics engine.
	PhysicsEngineInstance::Instance()->setGravity(btVector3(0, -9.81, 0));

	GUISystem::Initialise();

	if (!GUISystem::GetInstance().HasInitialised())
	{
		std::cout << "GUI not Initialised!" << std::endl;
	}

#if DEBUG_DRAW
#ifndef ORBIS
	PhysicsEngineInstance::Instance()->setDebugDrawer(DebugDraw::Instance());
	DebugDraw::Context(Renderer::GetInstance());
#endif
#endif
	GameObject* player = new GameObject("player");
	GameObject* ball = new GameObject("ball");
	GameObject* light1 = new GameObject("l");
	GameObject* light2 = new GameObject("l");

	GameObject* shooterAI = new GameObject("shooterAI");
	GameObject* goalieAI = new GameObject("goalieAI");

	RigidPhysicsObject* playerPhysics = new RigidPhysicsObject();
	playerPhysics->CreateCollisionShape(Vec3Physics(5.0, 2.5, 5.0), CUBOID);
	playerPhysics->CreatePhysicsBody(8.0, Vec3Physics(10, 5, 0), QuatPhysics(0, 0, 0, 1), Vec3Physics(1, 1, 1));

	RigidPhysicsObject* shooterAIPhysics = new RigidPhysicsObject();
	shooterAIPhysics->CreateCollisionShape(Vec3Physics(5.2, 2.0, 6.5), CUBOID);
	shooterAIPhysics->CreatePhysicsBody(8.0, Vec3Physics(-190, 5, 30), QuatPhysics(0, 0, 0, 1), Vec3Physics(1, 1, 1));
	shooterAIPhysics->GetPhysicsBody()->setRestitution(btScalar(0.9));
	shooterAIPhysics->GetPhysicsBody()->setFriction(0.5);
	shooterAIPhysics->GetPhysicsBody()->setRollingFriction(0.5);
	shooterAIPhysics->GetPhysicsBody()->setHitFraction(0.5);

	RigidPhysicsObject* goalieAIPhysics = new RigidPhysicsObject();
	goalieAIPhysics->CreateCollisionShape(Vec3Physics(5.2, 2.0, 6.5), CUBOID);
	goalieAIPhysics->CreatePhysicsBody(8.0, Vec3Physics(-230, 5, -30), QuatPhysics(0, 0, 0, 1), Vec3Physics(1, 1, 1));
	goalieAIPhysics->GetPhysicsBody()->setRestitution(btScalar(0.9));
	goalieAIPhysics->GetPhysicsBody()->setFriction(0.5);
	goalieAIPhysics->GetPhysicsBody()->setRollingFriction(0.5);
	goalieAIPhysics->GetPhysicsBody()->setHitFraction(0.5);

	RigidPhysicsObject* ballPhysics = new RigidPhysicsObject();
	ballPhysics->CreateCollisionShape(7.0);
	ballPhysics->CreatePhysicsBody(1.0, Vec3Physics(0, 3, 0), QuatPhysics(0, 0, 0, 1), Vec3Physics(1, 1, 1));
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
		std::cout << "Shader not operational!" << std::endl;

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

	// Create Stadium
	GameObject* stadium = new Stadium(material, netMaterial, "stadium");

	addGameObject(stadium);
	//myScene->addLightObject(light1);
	addLightObject(light2);

	player->SetRenderComponent(new RenderComponent(playerMaterial, ModelLoader::LoadMGL(MODEL_DIR"Car/car_base.mgl", true)));
	player->SetLocalTransform(Mat4Graphics::Translation(Vec3Graphics(0, -2.5, 0)) * Mat4Graphics::Scale(Vector3Simple(10, 10, 10)));
	player->SetPhysicsComponent(playerPhysics);
	player->GetPhysicsComponent()->GetPhysicsBody()->setRestitution(btScalar(0.9));
	player->GetPhysicsComponent()->GetPhysicsBody()->setFriction(0.5);
	player->GetPhysicsComponent()->GetPhysicsBody()->setRollingFriction(1);
	player->GetPhysicsComponent()->GetPhysicsBody()->setHitFraction(0.5);

	WheelObject* wheel = new WheelObject("fl");
	wheel->SetRenderComponent(new RenderComponent(playerMaterial, ModelLoader::LoadMGL(MODEL_DIR"Car/wheel.mgl", true)));
	wheel->SetLocalTransform(Mat4Graphics::Translation(Vec3Graphics(-0.45, 0.17, -0.45))) /*Mat4Graphics::Scale(Vector3Simple(10, 10, 10)))*/;
	player->AddChildObject(wheel);

	wheel = new WheelObject("bl");
	wheel->SetRenderComponent(new RenderComponent(playerMaterial, ModelLoader::LoadMGL(MODEL_DIR"Car/wheel.mgl", true)));
	wheel->SetLocalTransform(Mat4Graphics::Translation(Vec3Graphics(-0.45, 0.17, 0.45))) /*Mat4Graphics::Scale(Vector3Simple(10, 10, 10)))*/;
	player->AddChildObject(wheel);

	wheel = new WheelObject("fr");
	wheel->SetRenderComponent(new RenderComponent(playerMaterial, ModelLoader::LoadMGL(MODEL_DIR"Car/wheel.mgl", true)));
	wheel->SetLocalTransform(Mat4Graphics::Translation(Vec3Graphics(0.45, 0.17, -0.45))* Mat4Graphics::Scale(Vector3Simple(-1, -1, -1))) /*Mat4Graphics::Scale(Vector3Simple(10, 10, 10)))*/;
	player->AddChildObject(wheel);

	wheel = new WheelObject("br");
	wheel->SetRenderComponent(new RenderComponent(playerMaterial, ModelLoader::LoadMGL(MODEL_DIR"Car/wheel.mgl", true)));
	wheel->SetLocalTransform(Mat4Graphics::Translation(Vec3Graphics(0.45, 0.17, 0.45))* Mat4Graphics::Scale(Vector3Simple(-1, -1, -1))) /*Mat4Graphics::Scale(Vector3Simple(10, 10, 10)))*/;
	player->AddChildObject(wheel);

	ball->SetRenderComponent(new RenderComponent(ballMaterial, ModelLoader::LoadMGL(MODEL_DIR"Common/sphere.mgl", true)));
	ball->SetLocalTransform(Mat4Graphics::Scale(Vector3Simple(7, 7, 7)));
	ball->SetPhysicsComponent(ballPhysics);
	ball->GetPhysicsComponent()->GetPhysicsBody()->setRestitution(btScalar(0.9));
	ball->GetPhysicsComponent()->GetPhysicsBody()->setFriction(0.5);
	ball->GetPhysicsComponent()->GetPhysicsBody()->setRollingFriction(0.5);
	ball->GetPhysicsComponent()->GetPhysicsBody()->setHitFraction(0.5);

	shooterAI->SetRenderComponent(new RenderComponent(aiMaterial, ModelLoader::LoadMGL(MODEL_DIR"Car/car1.mgl", true)));
	shooterAI->SetLocalTransform(Mat4Graphics::Scale(Vector3Simple(10, 10, 10))* Mat4Graphics::Translation(Vector3Simple(0, -0.25f, 0)));
	shooterAI->SetPhysicsComponent(shooterAIPhysics);

	goalieAI->SetRenderComponent(new RenderComponent(ai2Material, ModelLoader::LoadMGL(MODEL_DIR"Car/car1.mgl", true)));
	goalieAI->SetLocalTransform(Mat4Graphics::Scale(Vector3Simple(10, 10, 10))* Mat4Graphics::Translation(Vector3Simple(0, -0.25f, 0)));
	goalieAI->SetPhysicsComponent(goalieAIPhysics);

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

	goalBallFilter.m_ballID = ballID;
	goalBallFilter.m_goal1ID = goal1ID;
	goalBallFilter.m_goal2ID = goal2ID;

	PhysicsEngineInstance::Instance()->getPairCache()->setOverlapFilterCallback(&goalBallFilter);

	ControllerComponent* cc = new ControllerComponent(player);
#ifndef ORBIS
	myControllers->setProducer(player);
#else
	myControllers->setProducer(player);
#endif

	////Define Orthographic Component
	//OrthoComponent* hudUI = new OrthoComponent(1.0f);
	////Add child GUI components, while defining materials, texture, and depth
	//hudUI->AddGUIComponent(new ScoreboardGUIComponent(guiMaterial, Texture::Get(TEXTURE_DIR"blue3.png"), 1.0));

	////Add Orthographic component to GUISystem
	//GUISystem::GetInstance().AddOrthoComponent(hudUI);

	attachCam(player);

	addGameObject(player);
	addGameObject(ball);
	addGameObject(shooterAI);
	addGameObject(goalieAI);

	addGameObject(goal1);
	addGameObject(goal2);
}


GameScene::~GameScene()
{
	PhysicsEngineInstance::Release();
	GUISystem::Destroy();

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