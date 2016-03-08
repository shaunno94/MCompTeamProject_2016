#pragma once
#include "ControllerComponent.h"
#include "Rendering\MenuGUI.h"

class MenuControllerComponent
{
public:
	MenuControllerComponent(MenuGUI* parent, unsigned int type);
	~MenuControllerComponent();
	void UpdateObject(float dt);

	void SelectNext();
	void SelectPrevious();
	void Submit();


protected:
	MenuGUI* m_parent;
};

