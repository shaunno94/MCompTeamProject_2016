#pragma once
#include "ControllerComponent.h"
class Controller
{
public:
	virtual void CheckInput() = 0;
	void setObject(ControllerComponent* object){ this->object = object; }
protected:
	ControllerComponent* object;

	 const float accel = 300000;
	 const float rotAccel = 1;
	 const float airAccel = 240000;
	 const float jump = 200000;
};

