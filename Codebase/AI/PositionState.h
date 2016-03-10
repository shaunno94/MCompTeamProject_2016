#pragma once
#include "State.h"
class GameObject;

class PositionState :
	public State
{
public:
	PositionState(StateMachine& stateMachine, GameObject& parent, GameObject& ball, GameObject& targetGoal);
	~PositionState();

	virtual void Start();
	virtual void Update(float dt);

protected:
	GameObject* m_ball;
	GameObject* m_targetGoal;
};

