#ifndef ORBIS
#pragma once
#include "MenuControllerComponent.h"
#include "Window.h"


class MenuController
{
public:
	MenuController(MenuControllerComponent* object);
	~MenuController();

	void CheckInput();
	void setObject(MenuControllerComponent* object){ this->object = object; }

protected:
	MenuControllerComponent* object;

};
#endif