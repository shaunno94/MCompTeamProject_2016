#include "LocalControlManager.h"
#include "AIControllerComponent.h"
#include "PS4Controller.h"


LocalControlManager::LocalControlManager()
{
	
}


LocalControlManager::~LocalControlManager()
{
}

void LocalControlManager::setProducer(GameObject* g, unsigned int type){
	ControllerComponent* cc = new ControllerComponent(g);
#ifndef ORBIS
	controllers.push_back(new KeyboardController(cc));
#else
	controllers.push_back(new PS4Controller(cc));
#endif

}

void LocalControlManager::setActor(GameObject* g, unsigned int type){
	ControllerComponent* cc = new AIControllerComponent(g, type);
	
}