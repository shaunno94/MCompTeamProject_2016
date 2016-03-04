#pragma once
#include "ControllerComponent.h"
class Controller
{
public:
	virtual void CheckInput() = 0;
	void setObject(ControllerComponent* object){ this->object = object; }
protected:
	ControllerComponent* object;

	const float accel = 14;
	const float rotAccel = 13;
	const float airAccel = 13;
	const float jump = 15;
};

