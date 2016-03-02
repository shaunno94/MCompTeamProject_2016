#include "ClearGoalState.h"
#include "LinearMath\btVector3.h"

ClearGoalState::ClearGoalState(StateMachine& stateMachine, GameObject& parent, GameObject& ball, GameObject& teamGoal) :
State(stateMachine, parent),
m_ball(&ball),
m_teamGoal(&teamGoal)
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
	btVector3 teamGoalPos = m_teamGoal->GetPhysicsComponent()->GetPhysicsBody()->getWorldTransform().getOrigin();
	btVector3 ballPos = m_ball->GetPhysicsComponent()->GetPhysicsBody()->getWorldTransform().getOrigin();
	btVector3 parentPos = m_parent->GetPhysicsComponent()->GetPhysicsBody()->getWorldTransform().getOrigin();

	btVector3 ballToGoal = teamGoalPos - ballPos;
	ballToGoal.setY(0);
	ballToGoal.normalize();

	btVector3 ballToAI = parentPos - ballPos;
	ballToAI.setY(0);
	ballToAI.normalize();

	btScalar dot = ballToGoal.dot(ballToAI);

	if (dot < 0.5) {
	}

	btVector3 direction = ballPos - parentPos;
	direction.normalize();
	direction *= 5.0f;

	m_parent->GetControllerComponent()->AddForce(direction.x(), 0.0f, direction.z());



}
