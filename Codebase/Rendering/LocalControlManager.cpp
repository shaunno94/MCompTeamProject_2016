#include "LocalControlManager.h"
#include "AIControllerComponent.h"


LocalControlManager::LocalControlManager()
{
	
}


LocalControlManager::~LocalControlManager()
{
}

void LocalControlManager::setProducer(GameObject* g){
	ControllerComponent* cc = new ControllerComponent(g);
	controllers.push_back(new KeyboardController(cc));
}

void LocalControlManager::setActor(GameObject* g, unsigned int type){
	ControllerComponent* cc = new AIControllerComponent(g, type);
	
}