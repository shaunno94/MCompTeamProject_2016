#include "AIControllerComponent.h"
#include "Renderer.h"
#include "AI\DistanceTrigger.h"
#include "AI\PositionState.h"
#include "AI\ShootState.h"
#include "AI\OnTargetTrigger.h"

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
	GameObject* goal = Renderer::GetInstance()->GetCurrentScene()->findGameObject("goal1");

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
		OnTargetTrigger* positionToShootOnTarget = new OnTargetTrigger();
		positionToShootOnTarget->setupTrigger(*m_parent, *ball, *goal);
		position->AddTrigger(positionToShootOnTarget, SHOOT);

		DistanceTrigger* shootToPosition = new DistanceTrigger();
		shootToPosition->setupTrigger(*m_parent, *ball, 15.0f, true);
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

void AIControllerComponent::AddForce(float x, float y, float z)
{
	float clamp = 0.6;

	Vec3Physics in(x, y, z);
	//in = getOrientation() * in;
	in.Normalize();

	Vec3Physics forward = getOrientation() * Vec3Physics(0, 0, 1);
	forward.Normalize();

	float newX = std::min(std::max(in.x, -clamp), clamp);
	float turningFactor = Lerp(0, 1, newX / clamp);
	AddTorque(0, 5 * turningFactor, 0);

	float dot = in.Dot(forward);
	if (dot>=0)
	{
		force = forward * 7;
	}
	else
	{
		force = -forward * 6;
	}

	force.y = 0;
}
