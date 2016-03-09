#ifndef ORBIS
#pragma once
#include "ControllerManager.h"
#include "Networking/Net.h"
#include "PCNetProducerController.h"

class NetClientControllerManager :
	public ControllerManager
{
public:
	NetClientControllerManager(NetSession* session);
	~NetClientControllerManager();

	virtual void setProducer(GameObject* g, unsigned int type) override;
	virtual void setActor(GameObject* g, unsigned int type) override;
	virtual void update(float ms) override;

protected:
	NetSession* m_session;

	//TODO: remove parent or remove the vector of simple controllers from parent as it is not needed
	std::vector<PCNetProducerController*> m_producingControllers;
	std::vector<PCNetController*> m_actorControllers;
};
#endif
