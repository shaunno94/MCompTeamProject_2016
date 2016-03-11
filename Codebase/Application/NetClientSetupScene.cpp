#include "NetClientSetupScene.h"
#include "Rendering\Renderer.h"
#include "Application\constants.h"


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
	if (guiSystem)
		delete guiSystem;
}

void ConnectingPrint()
{
}

void NetClientSetupScene::UpdateScene(float dt)
{
	for (auto element : connectionOrtho->GetElements())
		delete element;
	connectionOrtho->GetElements().clear();

	if (!m_IPGiven)
	{
		connectionOrtho->AddGUIComponent(new TextGUIComponent(guiMaterial, "Give Server address:", Vec3Graphics(-1.0f, -0.1f, 0), Vec3Graphics(0.04f, 0.04f, 1)));
		connectionOrtho->AddGUIComponent(new TextGUIComponent(guiMaterial, m_IP, Vec3Graphics(-1.0f, -0.2f, 0), Vec3Graphics(0.04f, 0.04f, 1)));

		if (m_IP.size() < 15)
		{
			for (char i = '0'; i <= '9'; i++)
			{
				if (Window::GetKeyboard()->KeyTriggered((KeyboardKeys)(KEYBOARD_0 + i - '0')))
				{
					m_IP += i;
				}
			}
			if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_PERIOD))
			{
				m_IP += '.';
			}
		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_BACK))
		{
			if (m_IP.size())
			{
				m_IP = m_IP.substr(0, m_IP.size() - 1);
			}
		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_RETURN))
		{
			m_IPGiven = true;
			NetworkGameData::Instance.client->ConnectToServer(m_IP);
		}
	}
	else
	{
		auto connectionState = NetworkGameData::Instance.client->GetConnection()->GetState();
		switch (connectionState)
		{
		case NetPeerConnected:
		{
			if (!NetworkGameData::Instance.client->GetSession())
			{
				if (NetworkGameData::Instance.client->GetSession())
				{
					auto* connections = NetworkGameData::Instance.client->GetSession()->GetMembers();
					size_t playerCount = connections->size();

					for (size_t i = 0; i < playerCount; ++i)
					{
						auto* connection = (*connections)[i];
						if (connection)
							connectionOrtho->AddGUIComponent(new TextGUIComponent(guiMaterial, connection->name, Vec3Graphics(-1.0f, (-0.1f * i), 0), Vec3Graphics(0.04f, 0.04f, 1)));
					}
				}
				else
				{
					connectionOrtho->AddGUIComponent(new TextGUIComponent(guiMaterial, "Waiting for session to start...", Vec3Graphics(-1.0f, (-0.1f * 0), 0), Vec3Graphics(0.04f, 0.04f, 1)));
				}
			}
			//accepted as member
			else
			{
				if (!(*NetworkGameData::Instance.client->GetSession()->GetMembers())[NetworkGameData::Instance.client->GetSessionMemberId()]->ready)
					NetworkGameData::Instance.client->Ready();
				if (NetworkGameData::Instance.client->GetSession()->GetState() == NetSessionState::NetSessionActive)
				{
					Renderer::GetInstance()->SetCurrentScene(Renderer::GetInstance()->GetScene(CLIENT_GAME_SCENE));
				}
			}
			break;
		}
		case NetPeerDisconnected:
		{
			Renderer::GetInstance()->SetCurrentScene(Renderer::GetInstance()->GetScene(MENU_SCENE));
			break;
		}
		default:
			connectionOrtho->AddGUIComponent(new TextGUIComponent(guiMaterial, "Connecting...", Vec3Graphics(-1.0f, -0.1f, 0), Vec3Graphics(0.04f, 0.04f, 1)));
			break;
		}
	}

	/*auto* connections = server->GetSession()->GetMembers();
	size_t playerCount = connections->size();

	for (size_t i = 0; i < playerCount; ++i)
	{
		auto* connection = (*connections)[i];
		if (connection)
			connectionOrtho->AddGUIComponent(new TextGUIComponent(guiMaterial, connection->GetAddressStr(), Vec3Graphics(-1.0f, (-0.1f * i), 0), Vec3Graphics(0.04f, 0.04f, 1)));
	}*/

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
	guiSystem->AddOrthoComponent(connectionOrtho);

}

void NetClientSetupScene::LoadAudio()
{

}

void NetClientSetupScene::SetupControls()
{
//	myControllers->setProducer(menuOrtho,2);
}


void NetClientSetupScene::Setup()
{
	//Network::Init();
	if (NetworkGameData::Instance.client)
	{
		delete NetworkGameData::Instance.client;
	}
	if (NetworkGameData::Instance.client)
		delete NetworkGameData::Instance.client;
	NetworkGameData::Instance.client = new NetClient();

	SetupShaders();
	SetupMaterials();
	SetupGameObjects();
	DrawGUI();
	SetupControls();

	m_IPGiven = false;

}

void NetClientSetupScene::Cleanup()
{
	//Network::Clear();
	m_IP = "";
}




