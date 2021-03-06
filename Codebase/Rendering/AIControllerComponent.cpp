#include "AIControllerComponent.h"
#include "Renderer.h"
#include "AI\DistanceTrigger.h"
#include "AI\PositionState.h"
#include "AI\ShootState.h"
#include "AI\OnTargetTrigger.h"
#include "AI\GuardGoalState.h"
#include "AI\ClearGoalState.h"
#include "AI\TimingTrigger.h"
#include "AI\AdvanceState.h"
#include "AI\ArenaHalfTrigger.h"
#include "AI\DefenceState.h"
#include "AI\constants.h"
#include "AI\AggressiveState.h"
#include "AI\OffenceState.h"

AIControllerComponent::AIControllerComponent(GameObject* parent, unsigned int type, bool blueTeam) :
	ControllerComponent(parent),
	m_type(type),
	m_blueTeam(blueTeam)
{
}

AIControllerComponent::~AIControllerComponent()
{
}

void AIControllerComponent::setupAI()
{
	GameObject* player = Renderer::GetInstance()->GetCurrentScene()->findGameObject("player");
	GameObject* ball = Renderer::GetInstance()->GetCurrentScene()->findGameObject("ball");

	
	GameObject* targetGoal = (m_blueTeam) ? Renderer::GetInstance()->GetCurrentScene()->findGameObject("redGoal") : Renderer::GetInstance()->GetCurrentScene()->findGameObject("blueGoal");
	GameObject* teamGoal = (m_blueTeam) ? Renderer::GetInstance()->GetCurrentScene()->findGameObject("blueGoal") : Renderer::GetInstance()->GetCurrentScene()->findGameObject("redGoal");

	m_StateMachine = new StateMachine();

	switch (m_type)
	{
	case SHOOTER:
	{

					// Create States
					PositionState* position = new PositionState(*m_StateMachine, *m_parent, *ball, *targetGoal);
					m_StateMachine->AddState(POSITION, position);

					ShootState* shoot = new ShootState(*m_StateMachine, *m_parent, *ball);
					m_StateMachine->AddState(SHOOT, shoot);

					// Create Triggers
					OnTargetTrigger* positionToShootOnTarget = new OnTargetTrigger();
					positionToShootOnTarget->setupTrigger(*m_parent, *ball, *targetGoal, 0.85f);
					position->AddTrigger(positionToShootOnTarget, SHOOT);

					TimingTrigger* shootToPositionTiming = new TimingTrigger();
					shootToPositionTiming->setupTrigger(*m_parent, 60);
					shoot->AddTrigger(shootToPositionTiming, POSITION);

					// Set active state
					m_StateMachine->ChangeState(POSITION);
	}
		break;
	case GOALKEEPER:
	{

					   // DEFENCE STATE (When ball is in team's half):
					   //		Contains Guard and Clear state
					   //		One transition to Advance single state
					   //		Starts on Guard, transition between Guard and Clear

					   GuardGoalState* guard = new GuardGoalState(*m_StateMachine, *m_parent, *ball, *teamGoal);
					   ClearGoalState* clearGoal = new ClearGoalState(*m_StateMachine, *m_parent, *ball);

					   DefenceState* defenceParentState = new DefenceState(*m_StateMachine, *m_parent, *ball, *teamGoal, *targetGoal);

					   defenceParentState->AddChildState(GUARD_GOAL, guard);
					   defenceParentState->AddChildState(CLEAR_GOAL, clearGoal);

					   defenceParentState->setupChildStates();
					   defenceParentState->Start();

					   ArenaHalfTrigger* toAdvanceTrigger = new ArenaHalfTrigger();
					   toAdvanceTrigger->setupTrigger(*m_parent, *ball, *teamGoal, false);
					   defenceParentState->AddTrigger(toAdvanceTrigger, ADVANCE);

					   m_StateMachine->AddState(DEFENCE, defenceParentState);

					   // ADVANCE STATE (When ball is in other half):
					   //		One Transition to Defence parent state

					   AdvanceState* advanceState = new AdvanceState(*m_StateMachine, *m_parent, *ball, *teamGoal);
					   m_StateMachine->AddState(ADVANCE, advanceState);

					   ArenaHalfTrigger* toDefenceTrigger = new ArenaHalfTrigger();
					   toDefenceTrigger->setupTrigger(*m_parent, *ball, *teamGoal, true);
					   advanceState->AddTrigger(toDefenceTrigger, DEFENCE);

					   // Default state is Defence (Guard Goal)
					   m_StateMachine->ChangeState(DEFENCE);

	}
		break;
	case AGGRESSIVE:
	{
					   // OFFENCE STATE (When player is away from ball):
					   //		Contains Position and Shoot state, effectively becoming a Shooter AI
					   //		One transition to Aggro single state
					   //		Starts on Position, transition between Position and Shoot

					   PositionState* position = new PositionState(*m_StateMachine, *m_parent, *ball, *targetGoal);
					   ShootState* shoot = new ShootState(*m_StateMachine, *m_parent, *ball);

					   OffenceState* offenceParentState = new OffenceState(*m_StateMachine, *m_parent, *ball, *targetGoal);
					   offenceParentState->AddChildState(POSITION, position);
					   offenceParentState->AddChildState(SHOOT, shoot);

					   offenceParentState->setupChildStates();
					   offenceParentState->Start();

					   DistanceTrigger* offenceToAggroTrigger = new DistanceTrigger();
					   offenceToAggroTrigger->setupTrigger(*player, *ball, 100.0f, true);
					   offenceParentState->AddTrigger(offenceToAggroTrigger, AGGRO);

					   m_StateMachine->AddState(OFFENCE, offenceParentState);


					   // AGGRO STATE (When player is within range of ball):
					   //		One Transition to Offense parent state

					   AggressiveState* aggroState = new AggressiveState(*m_StateMachine, *m_parent, *ball, *player);
					   m_StateMachine->AddState(AGGRO, aggroState);
					   m_StateMachine->ChangeState(AGGRO);

					   DistanceTrigger* aggroToOffenceTrigger = new DistanceTrigger();
					   aggroToOffenceTrigger->setupTrigger(*player, *ball, 170.0f, false);
					   aggroState->AddTrigger(aggroToOffenceTrigger, OFFENCE);

					   m_StateMachine->ChangeState(OFFENCE);
	}
		break;
	default:
		m_type = SHOOTER;
		setupAI();
		break;
	}
}

void AIControllerComponent::updateObject(float dt)
{
	if (m_parent->GetPhysicsComponent()->GetPhysicsBody()->getInterpolationLinearVelocity().length2() < 0.01)
	{
		m_inactiveFramesAgainstWall++;
	}
	else if (m_parent->GetPhysicsComponent()->GetPhysicsBody()->getInterpolationLinearVelocity().length2() > 0.05)
	{
		m_inactiveFramesAgainstWall = 0;
	}

	if (m_inactiveFramesAgainstWall > 60)
	{
		reset();
		m_inactiveFramesAgainstWall = 0;
	}

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
	Vec3Physics left = getOrientation() * Vec3Physics(-1, 0, 0);
	forward.Normalize();
	left.Normalize();

	/*auto rotForce = getOrientation() * in;
	rotForce.Normalize();
	float newX = std::min(std::max(rotForce.x, -clamp), clamp);
	float turningFactor = Lerp(0, 1, newX / clamp);*/

	float dot = in.Dot(left);

	AddTorque(0, 1 * (dot), 0);
	turnWheels(-dot);

	dot = in.Dot(forward);
	if (dot >= 0)
	{
		force = forward * 200000;
	}
	else
	{
		force = -forward * 200000;
	}

	force.y = 0;
}
