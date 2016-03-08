#ifndef ORBIS
#pragma once
#include "Controller.h"
#include "Window.h"

class MenuController :
	public Controller
{
public:
	MenuController(ControllerComponent* object);
	~MenuController();

	virtual void CheckInput();
};

#endif