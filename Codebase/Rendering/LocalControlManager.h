#pragma once
#include "ControllerManager.h"
class LocalControlManager :
	public ControllerManager
{
public:
	LocalControlManager();
	~LocalControlManager();
	virtual void setController(GameObject* g);
};

