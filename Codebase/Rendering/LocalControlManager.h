#pragma once
#include "ControllerManager.h"
#include "KeyboardController.h"
class LocalControlManager :
	public ControllerManager
{
public:
	LocalControlManager();
	~LocalControlManager();
	virtual void setProducer(GameObject* g);
	virtual void setActor(GameObject* g);
};

