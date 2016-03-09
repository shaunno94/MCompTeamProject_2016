#include "UIControllerManager.h"
#include "KeyboardController.h"
#include "PS4Controller.h"
#include "MenuControllerComponent.h"


UIControllerManager::UIControllerManager()
{
}


UIControllerManager::~UIControllerManager()
{
}

void UIControllerManager::update(float ms){
	for (auto ctrl : controllers)
	{
		ctrl->CheckInput(m_Scenes);
	}
}

void UIControllerManager::setProducer(MenuOrthoComponent* g, unsigned int type){
	MenuControllerComponent* cc = new MenuControllerComponent(g,type);
#ifndef ORBIS
	controllers.push_back(new MenuController(cc));
#else
	controllers.push_back(new PS4MenuController(cc));
#endif
}