#include "AIControllerComponent.h"
#include "Renderer.h"
#include "AI\DistanceTrigger.h"
#include "AI\PositionState.h"
#include "AI\ShootState.h"

enum AITypes
{
	SHOOTER = 0,
	GOALKEEPER = 1,
	AGGRESSIVE = 2
};

enum AIStates
{
	POSITION,
	SHOOT
};

AIControllerComponent::AIControllerComponent(GameObject* parent, unsigned int type) :
	ControllerComponent(parent)
{

	GameObject* ball = Renderer::GetInstance()->GetCurrentScene()->findGameObject("ball");

	switch (type)
	{
	case SHOOTER:
	{
		m_StateMachine = new StateMachine();

		// Create States
		PositionState* position = new PositionState(*m_StateMachine, *m_parent, *ball);
		m_StateMachine->AddState(POSITION, position);

		ShootState* shoot = new ShootState(*m_StateMachine, *m_parent, *ball);
		m_StateMachine->AddState(SHOOT, shoot);

		// Create Triggers
		DistanceTrigger* positionToShoot = new DistanceTrigger();
		positionToShoot->setupTrigger(*m_parent, *ball, 11.0f, true);
		position->AddTrigger(positionToShoot, SHOOT);


		DistanceTrigger* shootToPosition = new DistanceTrigger();
		shootToPosition->setupTrigger(*m_parent, *ball, 15.0f, false);
		shoot->AddTrigger(shootToPosition, POSITION);

		// Set active state
		m_StateMachine->ChangeState(POSITION);

	}
	break;
	case GOALKEEPER:
		break;
	case AGGRESSIVE:
		break;
	default:
		break;
	}

}

AIControllerComponent::~AIControllerComponent()
{
}

void AIControllerComponent::updateObject(float dt)
{
	m_StateMachine->Update(dt);
	ControllerComponent::updateObject(dt);
}
