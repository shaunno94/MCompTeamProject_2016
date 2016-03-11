#include "OffenceState.h"
#include "OnTargetTrigger.h"
#include "TimingTrigger.h"


OffenceState::OffenceState(StateMachine& stateMachine, GameObject& parent, GameObject& ball, GameObject& targetGoal) :
State(stateMachine, parent),
m_ball(&ball),
m_targetGoal(&targetGoal)
{
	m_activeChildState = POSITION;
}


OffenceState::~OffenceState()
{
}


void OffenceState::setupChildStates()
{
	// Create Triggers
	OnTargetTrigger* positionToShootOnTarget = new OnTargetTrigger();
	positionToShootOnTarget->setupTrigger(*m_parent, *m_ball, *m_targetGoal, 0.65f);

	TimingTrigger* shootToPositionTiming = new TimingTrigger();
	shootToPositionTiming->setupTrigger(*m_parent, 120);

	(*m_childStates)[POSITION]->AddTrigger(positionToShootOnTarget, SHOOT);
	(*m_childStates)[SHOOT]->AddTrigger(shootToPositionTiming, POSITION);
}

void OffenceState::Start()
{
	m_activeChildState = POSITION;
}

void OffenceState::Update(float dt)
{
	State::Update(dt);
}
