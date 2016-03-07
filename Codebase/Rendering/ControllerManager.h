#pragma once
#include "GameObject.h"
#include "Controller.h"
#include <vector>
class ControllerManager
{
public:
	ControllerManager();
	~ControllerManager();

	virtual void setProducer(GameObject* g, unsigned int type) = 0;
	virtual void setActor(GameObject* g, unsigned int type) = 0;
	virtual void update(float ms);
protected:
	std::vector<Controller*> controllers;
};

