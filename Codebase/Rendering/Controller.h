#pragma once
#include "ControllerComponent.h"
class Controller
{
public:
	virtual void CheckInput() = 0;
	void setObject(ControllerComponent* object){ this->object = object; }
protected:
	ControllerComponent* object;
};

