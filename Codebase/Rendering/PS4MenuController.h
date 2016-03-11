#ifdef ORBIS
#pragma once
#include "UIControllerComponent.h"
#include "Input/PS4Input.h"

class PS4MenuController
{
public:
	PS4MenuController(UIControllerComponent* object);
	~PS4MenuController();
	void CheckInput();
	void setObject(UIControllerComponent* object){ this->object = object; }

protected:
	UIControllerComponent* object;
};
#endif