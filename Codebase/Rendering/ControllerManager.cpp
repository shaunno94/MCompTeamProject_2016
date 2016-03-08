#include "ControllerManager.h"


ControllerManager::ControllerManager()
{
}


ControllerManager::~ControllerManager()
{
}

void ControllerManager::update(float ms){
	for (auto ctrl : controllers)
	{
		ctrl->CheckInput();
	}
}
