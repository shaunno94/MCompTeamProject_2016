#pragma once
#include "ControllerComponent.h"
#include "AI\StateMachine.h"
class AIControllerComponent :
	public ControllerComponent
{
public:
	AIControllerComponent(GameObject* parent, unsigned int type);
	~AIControllerComponent();

	virtual void updateObject(float dt) override;

protected:
	StateMachine* m_StateMachine;
};

