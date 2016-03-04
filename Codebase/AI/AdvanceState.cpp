#include "AdvanceState.h"


AdvanceState::AdvanceState(StateMachine& stateMachine, GameObject& parent, GameObject& ball, GameObject& teamGoal) :
State(stateMachine, parent),
m_ball(&ball),
m_teamGoal(&teamGoal)
{

}

AdvanceState::~AdvanceState()
{
}

void AdvanceState::Start()
{

}

void AdvanceState::Update(float dt)
{
	State::Update(dt);

	btVector3 teamGoalPos = m_teamGoal->GetPhysicsComponent()->GetPhysicsBody()->getWorldTransform().getOrigin();
	btVector3 ballPos = m_ball->GetPhysicsComponent()->GetPhysicsBody()->getWorldTransform().getOrigin();
	btVector3 parentPos = m_parent->GetPhysicsComponent()->GetPhysicsBody()->getWorldTransform().getOrigin();

	btVector3 desiredPos = btVector3(teamGoalPos.x() + 200, 0.0f, ballPos.z() * 0.7f);
	btVector3 direction = desiredPos - parentPos;
	direction.normalize();
	direction *= 5.0f;

	m_parent->GetControllerComponent()->AddForce(direction.x(), 0.0f, direction.z());
}
