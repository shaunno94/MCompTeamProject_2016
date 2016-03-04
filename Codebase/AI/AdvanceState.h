#pragma once
#include "State.h"
#include "Rendering/GameObject.h"

class AdvanceState :
	public State
{
public:
	AdvanceState(StateMachine& stateMachine, GameObject& parent, GameObject& ball, GameObject& teamGoal);
	~AdvanceState();

	virtual void Start();
	virtual void Update(float dt);

protected:
	GameObject* m_ball;
	GameObject* m_teamGoal;
};

