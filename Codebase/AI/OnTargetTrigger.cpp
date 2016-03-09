#include "OnTargetTrigger.h"

OnTargetTrigger::OnTargetTrigger()
{
}


OnTargetTrigger::~OnTargetTrigger()
{
}

void OnTargetTrigger::setupTrigger(GameObject& parent, GameObject& ball, GameObject& goal, float targetAccuracy)
{
	Trigger::setupTrigger(parent);
	m_ball = &ball;
	m_goal = &goal;
	m_targetAccuracy = targetAccuracy;
}

bool OnTargetTrigger::HasTriggered()
{
	btVector3 goalPos = m_goal->GetPhysicsComponent()->GetPhysicsBody()->getWorldTransform().getOrigin();
	btVector3 ballPos = m_ball->GetPhysicsComponent()->GetPhysicsBody()->getWorldTransform().getOrigin();
	btVector3 parentPos = m_parent->GetPhysicsComponent()->GetPhysicsBody()->getWorldTransform().getOrigin();

	btVector3 aiToBall = ballPos - parentPos;
	btVector3 ballToGoal = goalPos - ballPos;

	aiToBall.normalize();
	ballToGoal.normalize();

	btScalar dot = aiToBall.dot(ballToGoal);


	if (dot > m_targetAccuracy) {
		return true;
	}
	return false;
}
