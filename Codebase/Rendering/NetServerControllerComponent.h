#pragma once
#include "ControllerComponent.h"
#include "Rendering\MenuOrthoComponent.h"
#include "UIControllerComponent.h"

enum NETSERVER_SELECTION
{
	SERVER_MENU_BACK,
	SERVER_MENU_START
};

class NetServerControllerComponent : public UIControllerComponent
{
public:
	NetServerControllerComponent(MenuOrthoComponent* parent, unsigned int type);
	~NetServerControllerComponent();
	void UpdateObject(float dt);

	void SelectNext();
	void SelectPrevious();
	void Submit();


protected:
	MenuOrthoComponent* m_parent;
};

