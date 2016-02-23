#pragma once
#include "State.h"
class GameObject;

class RunAwayState :
	public State
{
public:
	RunAwayState(StateMachine& stateMachine, GameObject& parent, GameObject& runFromObj);
	~RunAwayState();

	virtual void Start();
	virtual void Update(float dt);

protected:
	GameObject* m_runFromObject;
};

