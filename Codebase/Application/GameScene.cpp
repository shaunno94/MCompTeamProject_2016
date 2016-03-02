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

	GameObject* ai1 = new GameObject("ai1");

	RigidPhysicsObject* playerPhysics = new RigidPhysicsObject();
	playerPhysics->CreateCollisionShape(Vec3Physics(5.0, 2.5, 5.0), CUBOID);
	playerPhysics->CreatePhysicsBody(8.0, Vec3Physics(10, 5, 0), QuatPhysics(0, 0, 0, 1), Vec3Physics(1, 1, 1));

	RigidPhysicsObject* wheelPhysics = new RigidPhysicsObject();
	wheelPhysics->CreateCollisionShape(Vec3Physics(1.5, 1.0, 1.5), CYLINDER);
	wheelPhysics->CreatePhysicsBody(8.0, Vec3Physics(5, 5, 5), QuatPhysics(0, 0, 0, 1), Vec3Physics(1, 1, 1));


	RigidPhysicsObject* aiPhysics = new RigidPhysicsObject();
	aiPhysics->CreateCollisionShape(Vec3Physics(5.0, 2.5, 5.0), CUBOID);
	aiPhysics->CreatePhysicsBody(8.0, Vec3Physics(30, 5, 10), QuatPhysics(0, 0, 0, 1), Vec3Physics(1, 1, 1));

	RigidPhysicsObject* ballPhysics = new RigidPhysicsObject();
	ballPhysics->CreateCollisionShape(7.0);
	ballPhysics->CreatePhysicsBody(2.0, Vec3Physics(0, 3, 0), QuatPhysics(0, 0, 0, 1), Vec3Physics(1, 1, 1));

	RigidPhysicsObject* floorPhysics = new RigidPhysicsObject();
	floorPhysics->CreateCollisionShape(0, Vec3Physics(0, 1, 0), true);
	floorPhysics->CreatePhysicsBody(0, Vec3Physics(0, -1, 0), QuatPhysics(0, 0, 0, 1));

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
	Material* guiMaterial = new Material(orthoShader);

	ballMaterial->Set(ReservedMeshTextures.DIFFUSE.name, Texture::Get(TEXTURE_DIR"checkerboard.tga", true));
	Material* playerMaterial = new Material(simpleShader);
	Material* aiMaterial = new Material(simpleShader);
	aiMaterial->Set(ReservedMeshTextures.DIFFUSE.name, Texture::Get(MODEL_DIR"car/body1.bmp", true));

	// Create Stadium
	GameObject* stadium = new Stadium(material, netMaterial, "stadium");

	addGameObject(stadium);
	//myScene->addLightObject(light1);
	addLightObject(light2);

	player->SetRenderComponent(new RenderComponent(playerMaterial, ModelLoader::LoadMGL(MODEL_DIR"Car/car1.mgl", true)));
	player->SetLocalTransform(Mat4Graphics::Scale(Vector3Simple(10, 10, 10)));
	player->SetPhysicsComponent(playerPhysics);
	player->GetPhysicsComponent()->GetPhysicsBody()->setRestitution(btScalar(0.9));
	player->GetPhysicsComponent()->GetPhysicsBody()->setFriction(0.5);
	player->GetPhysicsComponent()->GetPhysicsBody()->setRollingFriction(1);
	player->GetPhysicsComponent()->GetPhysicsBody()->setHitFraction(0.5);

	GameObject* wheel_fl = new GameObject();
	wheel_fl->SetRenderComponent(new RenderComponent(playerMaterial, ModelLoader::LoadMGL(MODEL_DIR"Car/wheel.mgl", true)));
	wheel_fl->SetLocalTransform(/*Mat4Graphics::Translation(Vec3Graphics(10,0,10))) */Mat4Graphics::Scale(Vector3Simple(10, 10, 10)));
	wheel_fl->SetPhysicsComponent(wheelPhysics);
	wheel_fl->GetPhysicsComponent()->GetPhysicsBody()->setRestitution(btScalar(0.9));
	wheel_fl->GetPhysicsComponent()->GetPhysicsBody()->setFriction(0.5);
	wheel_fl->GetPhysicsComponent()->GetPhysicsBody()->setRollingFriction(0.5);
	wheel_fl->GetPhysicsComponent()->GetPhysicsBody()->setHitFraction(0.5);

	ball->SetRenderComponent(new RenderComponent(ballMaterial, ModelLoader::LoadMGL(MODEL_DIR"Common/sphere.mgl", true)));
	ball->SetLocalTransform(Mat4Graphics::Scale(Vector3Simple(7, 7, 7)));
	ball->SetPhysicsComponent(ballPhysics);
	ball->GetPhysicsComponent()->GetPhysicsBody()->setRestitution(btScalar(0.9));
	ball->GetPhysicsComponent()->GetPhysicsBody()->setFriction(0.5);
	ball->GetPhysicsComponent()->GetPhysicsBody()->setRollingFriction(0.5);
	ball->GetPhysicsComponent()->GetPhysicsBody()->setHitFraction(0.5);

	ai1->SetRenderComponent(new RenderComponent(aiMaterial, ModelLoader::LoadMGL(MODEL_DIR"Car/car1.mgl", true)));
	ai1->SetLocalTransform(Mat4Graphics::Scale(Vector3Simple(10, 10, 10)));
	ai1->SetPhysicsComponent(aiPhysics);
	ai1->GetPhysicsComponent()->GetPhysicsBody()->setRestitution(btScalar(0.9));
	ai1->GetPhysicsComponent()->GetPhysicsBody()->setFriction(0.5);
	ai1->GetPhysicsComponent()->GetPhysicsBody()->setRollingFriction(0.5);
	ai1->GetPhysicsComponent()->GetPhysicsBody()->setHitFraction(0.5);

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

	attachCam(player);
	addGameObject(player);
	addGameObject(wheel_fl);
	addGameObject(ball);
	addGameObject(ai1);
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
	myControllers->setActor(Renderer::GetInstance()->GetCurrentScene()->findGameObject("ai1"), 0);
}