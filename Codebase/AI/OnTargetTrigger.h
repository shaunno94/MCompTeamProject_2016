#pragma once
#include "Trigger.h"
#include "LinearMath\btVector3.h"
#include "Rendering/GameObject.h"

class OnTargetTrigger :
	public Trigger
{
public:
	OnTargetTrigger();
	~OnTargetTrigger();

	virtual void setupTrigger(GameObject& parent, GameObject& ball, GameObject& goal);
	virtual bool HasTriggered();

protected:
	GameObject* m_ball;
	GameObject* m_goal;
};

