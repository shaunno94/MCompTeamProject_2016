#pragma once
#include "ControllerComponent.h"
#include "Rendering\MenuOrthoComponent.h"

class MenuControllerComponent
{
public:
	MenuControllerComponent(MenuOrthoComponent* parent, unsigned int type);
	~MenuControllerComponent();
	void UpdateObject(float dt);

	void SelectNext();
	void SelectPrevious();
	void Submit();


protected:
	MenuOrthoComponent* m_parent;
};

