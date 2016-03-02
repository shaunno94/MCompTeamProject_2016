#pragma once
#include "State.h"
#include "Rendering/GameObject.h"

class ClearGoalState :
	public State
{
public:
	ClearGoalState(StateMachine& stateMachine, GameObject& parent, GameObject& ball, GameObject& teamGoal);
	~ClearGoalState();

	virtual void Start();
	virtual void Update(float dt);

protected:
	GameObject* m_ball;
	GameObject* m_teamGoal;
};

