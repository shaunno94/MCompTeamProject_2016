#pragma once
#include "State.h"
#include "constants.h"
class OffenceState :
	public State
{
public:
	OffenceState(StateMachine& stateMachine, GameObject& parent, GameObject& ball, GameObject& targetGoal);
	~OffenceState();

	void setupChildStates();

	virtual void Start();
	virtual void Update(float dt);

protected:
	GameObject* m_ball;
	GameObject* m_targetGoal;
};

