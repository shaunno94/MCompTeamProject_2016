#include "ControllerManager.h"


ControllerManager::ControllerManager()
{
}


ControllerManager::~ControllerManager()
{
	for (auto ctrl : controllers)
	{
		if (ctrl)
			delete ctrl;
	}
	controllers.clear();
}

void ControllerManager::update(float ms){
	for (auto ctrl : controllers)
	{
		ctrl->CheckInput();
	}
}

void ControllerManager::clear(){
	controllers.clear();
}
