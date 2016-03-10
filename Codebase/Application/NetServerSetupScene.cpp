#include "NetServerSetupScene.h"


NetServerSetupScene::NetServerSetupScene()
{
	myControllers = new UIControllerManager();

	guiSystem = new GUISystem();

	if (!guiSystem->HasInitialised())
	{
		std::cout << "GUI not Initialised!" << std::endl;
	}
}


NetServerSetupScene::~NetServerSetupScene()
{
	delete btnMaterial;
	delete bgMaterial;
	delete guiMaterial;
	delete selectBtnMaterial;
	delete ipText;
	delete singleBtn;
	if (guiSystem)
		delete guiSystem;
}


void NetServerSetupScene::UpdateScene(float dt)
{
	m_Selected = menuOrtho->Update();

	for (auto element : connectionOrtho->GetElements())
		delete element;
	connectionOrtho->GetElements().clear();

	size_t size = server->GetConnectionCount();
	for (size_t i = 0; i < size; ++i)
	{
		NetConnectionDataInternal* connection = server->GetConnection(i);
		if (connection)
			server->AddToSession(server->GetConnection(i));
	}

	auto* connections = server->GetSession()->GetMembers();
	size_t playerCount = connections->size();

	for (size_t i = 0; i < playerCount; ++i)
	{
		auto* connection = (*connections)[i];
		if (connection)
			connectionOrtho->AddGUIComponent(new TextGUIComponent(guiMaterial, connection->GetAddressStr(), Vec3Graphics(-1.0f, (-0.1f * i), 0), Vec3Graphics(0.04f, 0.04f, 1)));
	}

	myControllers->update(dt);
}

void NetServerSetupScene::SetupGameObjects()
{

}

void NetServerSetupScene::SetupShaders()
{
#ifndef ORBIS
	orthoShader = new OGLShader(MENU_SERVER_VERT, MENU_SERVER_FRAG);
#else

#endif

	if (!orthoShader->IsOperational())
		std::cout << "Shader not opertational!" << std::endl;
}

void NetServerSetupScene::SetupMaterials()
{
	guiMaterial = new Material(orthoShader);
	bgMaterial = new Material(orthoShader);
	btnMaterial = new Material(orthoShader);
	selectBtnMaterial = new Material(orthoShader);

	//textMaterial = new Material(orthoShader);
}

void NetServerSetupScene::DrawGUI()
{
	ipOrtho = new OrthoComponent(1.0f);
	ipText = new TextGUIComponent(guiMaterial, *(ips[0]), Vec3Graphics(-1.0f, -0.5, 0), Vec3Graphics(0.04f, 0.04f, 1));
	ipOrtho->AddGUIComponent(ipText);

	connectionOrtho = new OrthoComponent(1.0f);

	menuOrtho = new MenuOrthoComponent(0.5);
	singleBtn = new ButtonGUIComponent(btnMaterial, selectBtnMaterial, Vec3Graphics(-0.7f, 0.7f, 0), Vec2Graphics(0.2f, 0.1f));

	menuOrtho->AddGUIComponent(singleBtn);

	guiSystem->AddOrthoComponent(ipOrtho);
	guiSystem->AddOrthoComponent(menuOrtho);
	guiSystem->AddOrthoComponent(connectionOrtho);
}

void NetServerSetupScene::LoadAudio()
{

}

void NetServerSetupScene::SetupControls()
{
	myControllers->setProducer(menuOrtho,1);
}

void NetServerSetupScene::Setup()
{
	Network::Init();
	server = new NetServer(10, 10, 10);

	ips = server->GetIpStr();

	SetupShaders();
	SetupMaterials();
	SetupGameObjects();
	DrawGUI();
	SetupControls();

}

void NetServerSetupScene::Cleanup()
{
	Network::Clear();
	delete server;
	server = nullptr;

	for (std::string* ip : ips)
	{
		delete ip;
	}
	ips.clear();
}


