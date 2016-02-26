#include "LocalControlManager.h"


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

void LocalControlManager::setActor(GameObject* g){
	ControllerComponent* cc = new ControllerComponent(g);
	//controllers.push_back(new AIController(cc));
}