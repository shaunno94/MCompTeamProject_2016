#pragma once
#include "ControllerComponent.h"
#include "Rendering\MenuGUI.h"

class MenuControllerComponent :
	public ControllerComponent
{
public:
	MenuControllerComponent(MenuGUI* parent, unsigned int type);
	~MenuControllerComponent();

protected:
	MenuGUI* menuGUI;
};

