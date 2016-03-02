#include "GuardGoalState.h"`
#include "LinearMath\btVector3.h"

GuardGoalState::GuardGoalState(StateMachine& stateMachine, GameObject& parent, GameObject& ball, GameObject& teamGoal) :
	State(stateMachine, parent),
	m_ball(&ball),
	m_teamGoal(&teamGoal)
{

}

GuardGoalState::~GuardGoalState()
{

}

void GuardGoalState::Start()
{

}

void GuardGoalState::Update(float dt)
{
	btVector3 teamGoalPos = m_teamGoal->GetPhysicsComponent()->GetPhysicsBody()->getWorldTransform().getOrigin();
	btVector3 ballPos = m_ball->GetPhysicsComponent()->GetPhysicsBody()->getWorldTransform().getOrigin();
	btVector3 parentPos = m_parent->GetPhysicsComponent()->GetPhysicsBody()->getWorldTransform().getOrigin();

	btVector3 goalToBall = ballPos - goalToBall;
	goalToBall.setY(0);
	goalToBall.normalize();
	goalToBall *= 15.0f;

	btVector3 desiredPos = (teamGoalPos + goalToBall) * btVector3(1,0,1);
	btVector3 direction = parentPos - desiredPos;
	direction.normalize();
	direction *= 3.0f;


	m_parent->GetControllerComponent()->AddForce(direction.x(), 0.0f, direction.z());



}
