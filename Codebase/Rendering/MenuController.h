#ifndef ORBIS
#pragma once
#include "MenuControllerComponent.h"
#include "Window.h"


class MenuController
{
public:
	MenuController(MenuControllerComponent* object);
	~MenuController();

	void CheckInput(std::vector<Scene*>& scenes);
	void setObject(MenuControllerComponent* object){ this->object = object; }

protected:
	MenuControllerComponent* object;

};
#endif