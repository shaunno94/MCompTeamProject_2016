#ifndef ORBIS
#include "NetServerSetupScene.h"
#include "constants.h"


NetServerSetupScene::NetServerSetupScene()
{
	myControllers = new UIControllerManager();

	guiSystem = new GUISystem();

	if (!guiSystem->HasInitialised())
	{
		std::cout << "GUI not Initialised!" << std::endl;
	}

	SetupShaders();
	SetupMaterials();
}


NetServerSetupScene::~NetServerSetupScene()
{
	if (guiSystem)
		delete guiSystem;
}


void NetServerSetupScene::UpdateScene(float dt)
{
	m_Selected = menuOrtho->Update();

	for (auto element : connectionOrtho->GetElements())
		delete element;
	connectionOrtho->GetElements().clear();

	while (NetworkGameData::Instance.server->GetSession()->GetMembers()->size() < NetworkGameData::posCount && NetworkGameData::Instance.server->GetConnectionCount() > 0)
	{
		NetConnectionDataInternal* connection = NetworkGameData::Instance.server->GetConnection(0);
		if (connection)
			NetworkGameData::Instance.server->AddToSession(NetworkGameData::Instance.server->GetConnection(0));
	}

	auto* connections = NetworkGameData::Instance.server->GetSession()->GetMembers();
	size_t playerCount = connections->size();

	for (size_t i = 0; i < playerCount; ++i)
	{
		auto* connection = (*connections)[i];
		if (connection)
			connectionOrtho->AddGUIComponent(new TextGUIComponent(guiMaterial, connection->GetAddressStr(), Vec3Graphics(-0.2f, (0.5f - (i*0.1)), 0), Vec3Graphics(0.04f, 0.04f, 1)));
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
	textMaterial = new Material(orthoShader);
}

void NetServerSetupScene::DrawGUI()
{
	ipOrtho = new OrthoComponent(1.0f);
	ipText = new TextGUIComponent(textMaterial, "Your IP Address:", Vec3Graphics(-1.0f, 0.7, 0), Vec3Graphics(0.04f, 0.04f, 1));
	ipAddress = new TextGUIComponent(textMaterial, *(ips[0]), Vec3Graphics(-0.2f, 0.7, 0), Vec3Graphics(0.04f, 0.04f, 1));
	sessionMemText = new TextGUIComponent(textMaterial, "Session Members:", Vec3Graphics(-1.0f, 0.5, 0), Vec3Graphics(0.04f, 0.04f, 1));
	ipOrtho->AddGUIComponent(ipAddress);
	ipOrtho->AddGUIComponent(ipText);
	ipOrtho->AddGUIComponent(sessionMemText);

	connectionOrtho = new OrthoComponent(1.0f);

	menuOrtho = new MenuOrthoComponent(0.5);

	startBtn = new ButtonGUIComponent(btnMaterial, selectBtnMaterial, Vec3Graphics(0.6f, -0.5f, 0.0f), Vec2Graphics(0.3f, 0.05f));
	startBtn->AddText(textMaterial, "Start");

	cancelBtn = new ButtonGUIComponent(btnMaterial, selectBtnMaterial, Vec3Graphics(0.6f, -0.7f, 0.0f), Vec2Graphics(0.3f, 0.05f));
	cancelBtn->AddText(textMaterial, "Cancel");

	menuOrtho->AddGUIComponent(cancelBtn);
	menuOrtho->AddGUIComponent(startBtn);
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
	//Network::Init();
	if (NetworkGameData::Instance.server)
		delete NetworkGameData::Instance.server;
	NetworkGameData::Instance.server = new NetServer(10, 10, 10);

	ips = NetworkGameData::Instance.server->GetIpStr();


	SetupGameObjects();
	DrawGUI();
	SetupControls();

}

void NetServerSetupScene::Cleanup()
{
	Scene::Cleanup();
	//Network::Clear();
	for (std::string* ip : ips)
	{
		delete ip;
	}
	ips.clear();
}

#endif
