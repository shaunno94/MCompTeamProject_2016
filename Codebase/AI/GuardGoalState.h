#pragma once
#include "State.h"
#include "Rendering/GameObject.h"
class GuardGoalState :
	public State
{
public:
	GuardGoalState(StateMachine& stateMachine, GameObject& parent, GameObject& ball, GameObject& teamGoal);
	~GuardGoalState();

	virtual void Start();
	virtual void Update(float dt);

protected:
	GameObject* m_ball;
	GameObject* m_teamGoal;
};

