#pragma once
#include "State.h"
class ShootState :
	public State
{
public:
	ShootState(StateMachine& stateMachine, GameObject& parent, GameObject& ball);
	~ShootState();

	virtual void Start();
	virtual void Update(float dt);

protected:
	GameObject* m_ball;
	GameObject* m_goal;
	bool m_hit;
};

