#ifdef ORBIS
#pragma once
#include "Controller.h"
#include "Input/PS4Input.h"

class PS4Controller :
	public Controller
{
public:
	PS4Controller(ControllerComponent* object);
	~PS4Controller();
	virtual void CheckInput();
};
#endif
