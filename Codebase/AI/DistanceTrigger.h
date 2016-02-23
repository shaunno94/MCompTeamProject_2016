#pragma once
#include "Trigger.h"

class GameObject;

class DistanceTrigger :
	public Trigger
{
public:
	DistanceTrigger();
	~DistanceTrigger();

	virtual void setupTrigger(GameObject& parent, GameObject& objB, float triggerDistance, bool triggerWhenLessThan = true);

	virtual bool HasTriggered();

protected:
	GameObject* m_objectB;
	float m_triggerDistance;
	bool m_triggerWhenLessThan;
};

