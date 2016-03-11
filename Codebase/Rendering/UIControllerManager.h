#pragma once
#include "Rendering\MenuOrthoComponent.h"
#ifndef ORBIS
#include "MenuController.h"
#else
#include "PS4MenuController.h"
typedef PS4MenuController MenuController;
#endif

class UIControllerManager
{
public:
	UIControllerManager();
	virtual ~UIControllerManager();

	virtual void setProducer(MenuOrthoComponent* g, unsigned int type);
	virtual void update(float ms);


protected:
	std::vector<MenuController*> controllers;
};