#include "DistanceTrigger.h"
#include <stdlib.h>


DistanceTrigger::DistanceTrigger()
{
	// Empty Constructor
}


DistanceTrigger::~DistanceTrigger()
{
	// Empty Destructor
}

void DistanceTrigger::setupTrigger(GameObject& objA, GameObject& objB, float distanceTriggered, bool lessThanDistance /* = true */)
{
	objectA = &objA;
	objectB = &objB;
	distance = distanceTriggered;
	lessThan = lessThanDistance;
}

bool DistanceTrigger::HasTriggered()
{
	if (lessThan) {
		if ((objectA->Physics()->GetPosition() - objectB->Physics()->GetPosition()).LengthSq() <= (distance * distance)) {
			return true;
		}
	}
	else {
		if ((objectA->Physics()->GetPosition() - objectB->Physics()->GetPosition()).LengthSq() >= (distance * distance)) {
			return true;
		}
	}
	return false;
	
}
