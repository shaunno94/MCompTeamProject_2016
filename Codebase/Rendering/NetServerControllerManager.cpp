#include "NetServerControllerManager.h"


NetServerControllerManager::NetServerControllerManager(NetSession* session)
{
	m_session = session;
}


NetServerControllerManager::~NetServerControllerManager()
{
}


void NetServerControllerManager::setProducer(GameObject* g, unsigned int type)
{
	ControllerComponent* cc = new ControllerComponent(g);
	m_producingControllers.push_back(new PCNetProducerController(cc, type));
}

void NetServerControllerManager::setActor(GameObject* g, unsigned int type)
{
	ControllerComponent* cc = new ControllerComponent(g);
	m_actorControllers.push_back(new PCNetController(cc, type));
}

void NetServerControllerManager::update(float ms)
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
		for (auto* actorController : m_actorControllers)
		{
			actorController->Sync(writer);
		}
	}
}