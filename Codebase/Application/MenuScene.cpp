#include "MenuScene.h"
#include "Stadium.h"
#include "BallGameObject.h"


MenuScene::MenuScene()
{
	myControllers = new UIControllerManager();

	guiSystem = new GUISystem();

	if (!guiSystem->HasInitialised())
	{
		std::cout << "GUI not Initialised!" << std::endl;
	}


	SetupShaders();
	SetupMaterials();
	DrawGUI();
	SetupControls();

	lightMesh = ModelLoader::LoadMGL(MODEL_DIR"Common/ico.mgl", true);
}


MenuScene::~MenuScene()
{
	delete btnMaterial;
	delete bgMaterial;
	delete guiMaterial;
	delete selectBtnMaterial;
	delete material;
	delete netMaterial;
	delete redPostMaterial;
	delete bluePostMaterial;
	delete ballMaterial;

	if (myControllers)
	{
		delete myControllers;
		myControllers = nullptr;
	}

	if (guiSystem)
		delete guiSystem;

	delete lightMesh;
}


void MenuScene::UpdateScene(float dt)
{
	player->SetLocalTransform(player->GetLocalTransform() * Mat4Graphics::Translation(playerTranslation) * Mat4Graphics::RotationY(-dt / 100.0f) * Mat4Graphics::Translation(-playerTranslation));
	m_Selected = menuOrtho->Update();
	myControllers->update(dt);
}

void MenuScene::SetupGameObjects()
{

	player = new GameObject("player");

	light2 = new GameObject("l");
	light2->SetRenderComponent(new RenderComponent(lightMaterial, lightMesh));
	light2->SetWorldTransform(Mat4Graphics::Translation(Vec3Graphics(600, 900, 600)) *Mat4Graphics::Scale(Vec3Graphics(2400, 2400, 2400)));
	light2->SetBoundingRadius(2400);

	ball = new BallGameObject("ball", ballMaterial);
	stadium = new Stadium(material, netMaterial, redPostMaterial, bluePostMaterial, "stadium");

	attachCam(player);
	getCamera()->autocam = true;

	addGameObject(player);
	addGameObject(stadium);
	addGameObject(ball);

	addLightObject(light2);
}

void MenuScene::SetupShaders()
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
	if (!simpleShader->IsOperational())
		std::cout << "Simple shader not operational!" << std::endl;
	if (!colourShader->IsOperational())
		std::cout << "Colour shader not operational!" << std::endl;
	if (!orthoShader->IsOperational())
		std::cout << "ortho shader not operational!" << std::endl;
}

void MenuScene::SetupMaterials()
{

	lightMaterial = new LightMaterial(pointlightShader);
	lightMaterial->shadowType = _2D;

	guiMaterial = new Material(orthoShader);
	bgMaterial = new Material(orthoShader);
	btnMaterial = new Material(orthoShader);
	selectBtnMaterial = new Material(orthoShader);

	material = new Material(simpleShader);
	netMaterial = new Material(simpleShader, true);
	ballMaterial = new Material(simpleShader);
	redPostMaterial = new ExtendedMaterial(colourShader, true);
	bluePostMaterial = new ExtendedMaterial(colourShader, true);

	//textMaterial = new Material(orthoShader);
}

void MenuScene::DrawGUI()
{
	bgOrtho = new OrthoComponent(1.0);
	//menuBg = new MenuBackgroundGUI(bgMaterial);
	//bgOrtho->AddGUIComponent(menuBg);

	menuOrtho = new MenuOrthoComponent(0.5);
	singleBtn = new ButtonGUIComponent(btnMaterial, selectBtnMaterial, Vec3Graphics(-0.7f, 0.7f, 0), Vec2Graphics(0.2f, 0.1f));
	multiBtn = new ButtonGUIComponent(btnMaterial, selectBtnMaterial, Vec3Graphics(-0.7f, 0.4f, 0), Vec2Graphics(0.2f, 0.1f));
	exitBtn = new ButtonGUIComponent(btnMaterial, selectBtnMaterial, Vec3Graphics(-0.7f, 0.1f, 0), Vec2Graphics(0.2f, 0.1f));
	titleImg = new MenuBackgroundGUI(bgMaterial);

	menuOrtho->AddGUIComponent(singleBtn);
	menuOrtho->AddGUIComponent(multiBtn);
	menuOrtho->AddGUIComponent(exitBtn);
	bgOrtho->AddGUIComponent(titleImg);

	guiSystem->AddOrthoComponent(bgOrtho);
	guiSystem->AddOrthoComponent(menuOrtho);
}

void MenuScene::LoadAudio()
{

}

void MenuScene::SetupControls()
{
	myControllers->setProducer(menuOrtho,0);
}

void MenuScene::Cleanup()
{
	Scene::Cleanup();
	ClearObjects();
}

void MenuScene::Setup()
{
	Scene::Setup();

	SetupGameObjects();
}



