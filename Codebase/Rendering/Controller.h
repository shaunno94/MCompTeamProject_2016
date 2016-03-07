#pragma once
#include "ControllerComponent.h"
class Controller
{
public:
	virtual void CheckInput() = 0;
	void setObject(ControllerComponent* object){ this->object = object; }
protected:
	ControllerComponent* object;

	 float accel = 240000;
	 float rotAccel = 1;
	 float airAccel = 140000;
	 float jump = 150000;
};

