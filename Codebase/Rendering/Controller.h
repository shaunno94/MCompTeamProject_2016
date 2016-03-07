#pragma once
#include "ControllerComponent.h"
class Controller
{
public:
	virtual void CheckInput() = 0;
	void setObject(ControllerComponent* object){ this->object = object; }
protected:
	ControllerComponent* object;

	const float accel = 100000;
	const float rotAccel = 100000;
	const float airAccel = 100000;
	const float jump = 120000;
};

