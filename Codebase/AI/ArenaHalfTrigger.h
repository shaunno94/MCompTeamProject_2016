#pragma once
#include "Trigger.h"
#include "Rendering/GameObject.h"

class GameObject;

class ArenaHalfTrigger :
	public Trigger
{
public:
	ArenaHalfTrigger();
	~ArenaHalfTrigger();

	virtual void setupTrigger(GameObject& parent, GameObject& ball, GameObject& teamGoal, bool triggerInTeamHalf = true);

	virtual bool HasTriggered();

protected:
	GameObject* m_teamGoal;
	GameObject* m_ball;
	bool m_triggerInTeamHalf;
};

