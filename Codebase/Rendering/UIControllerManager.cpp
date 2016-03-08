#include "UIControllerManager.h"
#include "KeyboardController.h"
#include "PS4Controller.h"


UIControllerManager::UIControllerManager()
{
}


UIControllerManager::~UIControllerManager()
{
}

void UIControllerManager::update(float ms){
	for (auto ctrl : controllers)
	{
		ctrl->CheckInput();
	}
}

void UIControllerManager::setProducer(MenuGUI* g, unsigned int type){
	ControllerComponent* cc = new ControllerComponent(g);
#ifndef ORBIS
	controllers.push_back(new KeyboardController(cc));
#else
	controllers.push_back(new PS4Controller(cc));
#endif

}