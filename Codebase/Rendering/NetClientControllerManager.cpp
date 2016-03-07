#include "NetClientControllerManager.h"
#include "Networking/Net.h"
#include "Networking/NetClient.h"

NetClientControllerManager::NetClientControllerManager(NetSession* session)
{
	m_session = session;
}


NetClientControllerManager::~NetClientControllerManager()
{
}


void NetClientControllerManager::setProducer(GameObject* g, unsigned int type)
{
	ControllerComponent* cc = new ControllerComponent(g);
	m_producingControllers.push_back(new PCNetProducerController(cc, type));
}

void NetClientControllerManager::setActor(GameObject* g, unsigned int type)
{
	ControllerComponent* cc = new ControllerComponent(g);
	m_actorControllers.push_back(new PCNetController(cc, type));
}

void NetClientControllerManager::update(float ms)
{
	{
		NetSessionReader reader(m_session->getRecvBuffer());
		if (reader.GetMessages())
		{
			for (auto* actorController : m_actorControllers)
			{
				actorController->Sync(reader);
			}
		}
	}
	{
		NetSessionWriter writer(m_session->getSendBuffer());
		for (auto* producerController : m_producingControllers)
		{
			producerController->Sync(writer);
		}
	}
}