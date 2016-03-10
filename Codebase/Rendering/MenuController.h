#ifndef ORBIS
#pragma once
#include "UIControllerComponent.h"
#include "Window.h"


class MenuController
{
public:
	MenuController(UIControllerComponent* object);
	~MenuController();

	void CheckInput();
	void setObject(UIControllerComponent* object){ this->object = object; }

protected:
	UIControllerComponent* object;

};
#endif