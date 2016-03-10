#include "LocalControlManager.h"
#include "PS4Controller.h"
#include "PS4MenuController.h"
#include "MenuControllerComponent.h"

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
	//TODO: Change team to be field of GameObject class
	ControllerComponent* cc = new AIControllerComponent(g, type, true);
	aiControllers.push_back(dynamic_cast<AIControllerComponent*>(cc));
}
	
void LocalControlManager::setupActors()
{
	for (AIControllerComponent* ai : aiControllers) {
		ai->setupAI();
	}
}