#pragma once
#include "Trigger.h"

class DistanceTrigger :
	public Trigger
{
public:
	DistanceTrigger();
	~DistanceTrigger();

	virtual void setupTrigger(GameObject& objA, GameObject& objB, float distanceTriggered, bool lessThanDistance = true);

	virtual bool HasTriggered();

protected:
	GameObject* objectA;
	GameObject* objectB;
	float distance;
	bool lessThan;
};

