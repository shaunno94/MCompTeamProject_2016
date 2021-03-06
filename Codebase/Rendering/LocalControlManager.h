#pragma once
#include "ControllerManager.h"
#include "KeyboardController.h"
#include "MenuController.h"
#include "AIControllerComponent.h"
class LocalControlManager :
	public ControllerManager
{
public:
	LocalControlManager();
	~LocalControlManager();
	virtual void setProducer(GameObject* g, unsigned int type) override;
	virtual void setActor(GameObject* g, unsigned int type) override;

	void setupActors();
	void clear() override;

private:
	std::vector<AIControllerComponent*> aiControllers;
};

