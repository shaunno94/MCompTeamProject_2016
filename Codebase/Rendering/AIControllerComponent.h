#pragma once
#include "ControllerComponent.h"
#include "AI\StateMachine.h"
class AIControllerComponent :
	public ControllerComponent
{
public:
	AIControllerComponent(GameObject* parent, unsigned int type, bool blueTeam);
	~AIControllerComponent();

	void setupAI();

	virtual void AddForce(float x, float y, float z) override;

	virtual void updateObject(float dt) override;

//	void CreateForce(btVector3& parentPos, btVector3& dest);

protected:
	unsigned int m_type;
	StateMachine* m_StateMachine;
	unsigned int m_inactiveFramesAgainstWall;
	bool m_blueTeam;
};
