#include "NetClientSetupScene.h"


NetClientSetupScene::NetClientSetupScene()
{
	myControllers = new UIControllerManager();

	guiSystem = new GUISystem();
	
	if (!guiSystem->HasInitialised())
	{
		std::cout << "GUI not Initialised!" << std::endl;
	}


	SetupShaders();
	SetupMaterials();
	SetupGameObjects();
	DrawGUI();
	SetupControls();
}


NetClientSetupScene::~NetClientSetupScene()
{
	delete btnMaterial;
	delete bgMaterial;
	delete guiMaterial;
	delete selectBtnMaterial;
	if (guiSystem)
		delete guiSystem;
}


void NetClientSetupScene::UpdateScene(float dt)
{
	m_Selected = menuOrtho->Update();

	//for (auto element : connectionOrtho->GetElements())
	//	delete element;
	//connectionOrtho->GetElements().clear();

	//size_t size = server->GetConnectionCount();
	//for (size_t i = 0; i < size; ++i)
	//{
	//	NetConnectionDataInternal* connection = server->GetConnection(i);
	//	if (connection)
	//		server->AddToSession(server->GetConnection(i));
	//}

	//auto* connections = server->GetSession()->GetMembers();
	//size_t playerCount = connections->size();

	//for (size_t i = 0; i < playerCount; ++i)
	//{
	//	auto* connection = (*connections)[i];
	//	if (connection)
	//		connectionOrtho->AddGUIComponent(new TextGUIComponent(guiMaterial, connection->GetAddressStr(), Vec3Graphics(-1.0f, (-0.1f * i), 0), Vec3Graphics(0.04f, 0.04f, 1)));
	//}

	myControllers->update(dt);
}

void NetClientSetupScene::SetupGameObjects()
{

}

void NetClientSetupScene::SetupShaders()
{
#ifndef ORBIS
	orthoShader = new OGLShader(MENU_CLIENT_VERT, MENU_CLIENT_FRAG);
#else

#endif

	if (!orthoShader->IsOperational())
		std::cout << "Shader not opertational!" << std::endl;
}

void NetClientSetupScene::SetupMaterials()
{
	guiMaterial = new Material(orthoShader);
	bgMaterial = new Material(orthoShader);
	btnMaterial = new Material(orthoShader);
	selectBtnMaterial = new Material(orthoShader);

	//textMaterial = new Material(orthoShader);
}

void NetClientSetupScene::DrawGUI()
{
	connectionOrtho = new OrthoComponent(1.0f);
	
	menuOrtho = new MenuOrthoComponent(0.5);
	singleBtn = new ButtonGUIComponent(btnMaterial, selectBtnMaterial, Vec3Graphics(-0.7f, 0.7f, 0), Vec2Graphics(0.2f, 0.1f));
	multiBtn = new ButtonGUIComponent(btnMaterial, selectBtnMaterial, Vec3Graphics(-0.7f, 0.4f, 0), Vec2Graphics(0.2f, 0.1f));
	exitBtn = new ButtonGUIComponent(btnMaterial, selectBtnMaterial, Vec3Graphics(-0.7f, 0.1f, 0), Vec2Graphics(0.2f, 0.1f));

	menuOrtho->AddGUIComponent(singleBtn);
	menuOrtho->AddGUIComponent(multiBtn);
	menuOrtho->AddGUIComponent(exitBtn);

	guiSystem->AddOrthoComponent(connectionOrtho);

	guiSystem->AddOrthoComponent(menuOrtho);
}

void NetClientSetupScene::LoadAudio()
{

}

void NetClientSetupScene::SetupControls()
{
	myControllers->setProducer(menuOrtho,2);
}




