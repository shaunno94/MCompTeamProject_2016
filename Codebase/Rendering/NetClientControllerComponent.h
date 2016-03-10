#pragma once
#include "ControllerComponent.h"
#include "Rendering\MenuOrthoComponent.h"
#include "UIControllerComponent.h"

enum NETCLIENT_SELECTION
{
	CLIENT_MENU_BACK,
	CLIENT_MENU_START
};

class NetClientControllerComponent : public UIControllerComponent
{
public:
	NetClientControllerComponent(MenuOrthoComponent* parent, unsigned int type);
	~NetClientControllerComponent();
	void UpdateObject(float dt);

	void SelectNext();
	void SelectPrevious();
	void Submit();


protected:
	MenuOrthoComponent* m_parent;
};

