#pragma once
#include "State.h"
#include "Rendering/GameObject.h"

class AggressiveState :
	public State
{
public:
	AggressiveState(StateMachine& stateMachine, GameObject& parent, GameObject& ball, GameObject& player);
	~AggressiveState();

	virtual void Start();
	virtual void Update(float dt);

protected:
	GameObject* m_ball;
	GameObject* m_player;
	RigidPhysicsObject* m_playerPhys;
};

