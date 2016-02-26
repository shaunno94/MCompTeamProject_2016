#include "LocalControlManager.h"


LocalControlManager::LocalControlManager()
{
}


LocalControlManager::~LocalControlManager()
{
}

void LocalControlManager::setController(GameObject* g){
	ControllerComponent* cc = new ControllerComponent(g);
}