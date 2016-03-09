#pragma once
#include "Rendering\MenuOrthoComponent.h"
#include "MenuController.h"


class UIControllerManager
{
public:
	UIControllerManager();
	~UIControllerManager();

	virtual void setProducer(MenuOrthoComponent* g, unsigned int type);
	virtual void update(float ms);

protected:
	std::vector<MenuController*> controllers;
};