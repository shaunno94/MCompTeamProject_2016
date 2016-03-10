#pragma once
#include "ControllerComponent.h"
#include "Rendering\MenuOrthoComponent.h"
#include "UIControllerComponent.h"

enum MENU_SELECTION
{
	SINGLE_PLAYER,
	MULTIPLAYER,
	QUIT
};

class MenuControllerComponent : public UIControllerComponent
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

