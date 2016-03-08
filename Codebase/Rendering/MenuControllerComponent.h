#pragma once
#include "ControllerComponent.h"
#include "Rendering\MenuGUI.h"
#include "Rendering\MenuGUI.h"

class MenuControllerComponent :
	public ControllerComponent
{
public:
	MenuControllerComponent(GameObject* parent, unsigned int type);
	~MenuControllerComponent();

protected:

	MenuGUI* menuGUI;
};

