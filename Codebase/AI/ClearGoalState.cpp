#include "ClearGoalState.h"
#include "LinearMath\btVector3.h"

ClearGoalState::ClearGoalState(StateMachine& stateMachine, GameObject& parent, GameObject& ball) :
State(stateMachine, parent),
m_ball(&ball)
{

}

ClearGoalState::~ClearGoalState()
{

}

void ClearGoalState::Start()
{

}

void ClearGoalState::Update(float dt)
{
	State::Update(dt);

	btVector3 ballPos = m_ball->GetPhysicsComponent()->GetPhysicsBody()->getWorldTransform().getOrigin();
	btVector3 parentPos = m_parent->GetPhysicsComponent()->GetPhysicsBody()->getWorldTransform().getOrigin();

	btVector3 direction = parentPos - ballPos;
	direction.normalize();
	direction *= 5.0f;

	m_parent->GetControllerComponent()->AddForce(direction.x(), 0.0f, direction.z());

}
