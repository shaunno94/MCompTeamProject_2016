#pragma once
#include "Controller.h"
#include "Window.h"

class KeyboardController :
	public Controller
{
public:
	KeyboardController(ControllerComponent* object);
	~KeyboardController();
	virtual void CheckInput();
};

