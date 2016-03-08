#ifdef ORBIS
#pragma once
#include "Controller.h"
#include "Input/PS4Input.h"

class PS4MenuController :
	public Controller
{
public:
	PS4MenuController(ControllerComponent* object);
	~PS4MenuController();
	virtual void CheckInput();
};
#endif