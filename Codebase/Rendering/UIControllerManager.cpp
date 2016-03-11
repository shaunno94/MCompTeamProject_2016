#include "UIControllerManager.h"
#include "KeyboardController.h"
#include "PS4Controller.h"
#include "MenuControllerComponent.h"
#include "NetServerControllerComponent.h"
#include "NetClientControllerComponent.h"


UIControllerManager::UIControllerManager()
{
}


UIControllerManager::~UIControllerManager()
{
}

void UIControllerManager::update(float ms)
{
	for (auto ctrl : controllers)
	{
		ctrl->CheckInput();
	}
}

void UIControllerManager::setProducer(MenuOrthoComponent* g, unsigned int type)
{
	UIControllerComponent* cc = nullptr;
	switch (type)
	{
	case 0:
		cc = new MenuControllerComponent(g, type);
		break;
	case 1:
		cc = new NetServerControllerComponent(g, type);
		break;
	case 2:
		cc = new NetClientControllerComponent(g, type);
	}
#ifndef ORBIS
		controllers.push_back(new MenuController(cc));
#else
		controllers.push_back(new PS4MenuController(cc));
#endif
}