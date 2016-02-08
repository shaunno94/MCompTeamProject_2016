#pragma once
#include "State.h"
class GameObject;

class ChaseState :
	public State
{
public:
	ChaseState(StateMachine& stateMachine, GameObject& parent, GameObject& chaseObj);
	~ChaseState();

	virtual void Start();
	virtual void Update(float dt);

protected:
	GameObject* m_chaseObject;
};

