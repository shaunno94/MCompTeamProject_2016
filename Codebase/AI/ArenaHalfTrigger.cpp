#include "ArenaHalfTrigger.h"
#include "LinearMath\btVector3.h"

ArenaHalfTrigger::ArenaHalfTrigger()
{
}

ArenaHalfTrigger::~ArenaHalfTrigger()
{
}

void ArenaHalfTrigger::setupTrigger(GameObject& parent, GameObject& ball, GameObject& teamGoal, bool triggerInTeamHalf /*= true*/)
{
	Trigger::setupTrigger(parent);
	m_teamGoal = &teamGoal;
	m_triggerInTeamHalf = triggerInTeamHalf;
	m_ball = &ball;
}

bool ArenaHalfTrigger::HasTriggered()
{
	btVector3 goalPos = m_teamGoal->GetPhysicsComponent()->GetPhysicsBody()->getWorldTransform().getOrigin();
	btVector3 ballPos = m_ball->GetPhysicsComponent()->GetPhysicsBody()->getWorldTransform().getOrigin();

	if (m_triggerInTeamHalf) {
		if ((goalPos.x() < 0) 
			&& (ballPos.x() < 0)) {
			return true;
		}
		else if ((goalPos.x() > 0) 
			&& (ballPos.x() > 0)) {
			return true;
		}
	}
	else {
		if ((goalPos.x() < 0) 
			&& (ballPos.x() > 0)) {
			std::cout << m_parent->GetName() << ": DEFENCE -> ADVANCE " << std::endl;
			return true;
		}
		else if ((goalPos.x() > 0) 
			&& (ballPos.x() < 0)) {
			std::cout << m_parent->GetName() << ": DEFENCE -> ADVANCE " << std::endl;
			return true;
		}
	}

	return false;
}
