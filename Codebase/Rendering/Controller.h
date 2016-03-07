#pragma once
#include "ControllerComponent.h"
class Controller
{
public:
	virtual void CheckInput() = 0;
	void setObject(ControllerComponent* object){ this->object = object; }
protected:
	ControllerComponent* object;

	const float accel = 180000;
	const float rotAccel = 120000;
	const float airAccel = 120000;
	const float jump = 150000;
};

