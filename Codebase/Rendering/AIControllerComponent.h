#pragma once
#include "ControllerComponent.h"
#include "AI\StateMachine.h"
class AIControllerComponent :
	public ControllerComponent
{
public:
	AIControllerComponent(GameObject* parent, unsigned int type);
	~AIControllerComponent();

	virtual void AddForce(float x, float y, float z) override;

	virtual void updateObject(float dt) override;

protected:
	StateMachine* m_StateMachine;
};