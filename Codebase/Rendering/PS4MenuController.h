#ifdef ORBIS
#pragma once
#include "MenuControllerComponent.h"
#include "Input/PS4Input.h"

class PS4MenuController
{
public:
	PS4MenuController(MenuControllerComponent* object);
	~PS4MenuController();
	void CheckInput();
	void setObject(MenuControllerComponent* object){ this->object = object; }

protected:
	MenuControllerComponent* object;
};
#endif