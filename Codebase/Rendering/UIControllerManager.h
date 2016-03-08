#pragma once
#include "Rendering\MenuGUI.h"
#include "MenuController.h"


class UIControllerManager
{
public:
	UIControllerManager();
	~UIControllerManager();

	virtual void setProducer(MenuGUI* g, unsigned int type);
	virtual void update(float ms);

protected:
	std::vector<MenuController*> controllers;
};