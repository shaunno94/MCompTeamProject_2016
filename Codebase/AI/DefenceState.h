#pragma once
#include "State.h"
class DefenceState :
	public State
{
public:
	DefenceState(StateMachine& stateMachine, GameObject& parent, GameObject& ball, GameObject& teamGoal, GameObject& targetGoal);
	~DefenceState();

	void setupChildStates();

	virtual void Start();
	virtual void Update(float dt);

protected:
	GameObject* m_ball;
	GameObject* m_teamGoal;
	GameObject* m_targetGoal;
};

