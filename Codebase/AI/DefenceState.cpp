#include "DefenceState.h"
#include "OnTargetTrigger.h"
#include "DistanceTrigger.h"

DefenceState::DefenceState(StateMachine& stateMachine, GameObject& parent, GameObject& ball, GameObject& teamGoal, GameObject& targetGoal) :
State(stateMachine, parent),
m_ball(&ball),
m_teamGoal(&teamGoal),
m_targetGoal(&targetGoal)
{
	m_activeChildState = GUARD_GOAL;
}

DefenceState::~DefenceState()
{
}

void DefenceState::setupChildStates()
{
	DistanceTrigger* guardToClear = new DistanceTrigger();
	guardToClear->setupTrigger(*m_ball, *m_teamGoal, 300, false);

	DistanceTrigger* clearToGuard = new DistanceTrigger();
	clearToGuard->setupTrigger(*m_ball, *m_teamGoal, 230, true);

	(*m_childStates)[GUARD_GOAL]->AddTrigger(guardToClear, CLEAR_GOAL);
	(*m_childStates)[CLEAR_GOAL]->AddTrigger(clearToGuard, GUARD_GOAL);
}

void DefenceState::Start()
{
	m_activeChildState = GUARD_GOAL;
}

void DefenceState::Update(float dt)
{
	State::Update(dt);
}
