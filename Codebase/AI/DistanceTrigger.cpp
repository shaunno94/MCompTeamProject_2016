#include "DistanceTrigger.h"
#include "Rendering/GameObject.h"
#include <stdlib.h>


DistanceTrigger::DistanceTrigger()
{
	// Empty Constructor
}


DistanceTrigger::~DistanceTrigger()
{
}

/// <summary>
///		Set up the Distance Trigger
/// </summary>
/// <param name="parent"> Parent GameObject </param>
/// <param name="objB"> Object to maintain distance from </param>
/// <param name="triggerDistance"> Distance to keep before trigger is triggered </param>
/// <param name="triggerWhenLessThan"> Boolean value for whether it triggers when the distance becomes less than or greater than 
///		specified value </param>
void DistanceTrigger::setupTrigger(GameObject& parent, GameObject& objB, float triggerDistance, bool triggerWhenLessThan /* = true */)
{
	Trigger::setupTrigger(parent);
	m_objectB = &objB;
	m_triggerDistance = triggerDistance;
	m_triggerWhenLessThan = triggerWhenLessThan;
}

bool DistanceTrigger::HasTriggered()
{
	btVector3 parentOrigin = m_parent->GetPhysicsComponent()->GetPhysicsBody()->getWorldTransform().getOrigin()* btVector3(1, 0, 1);
	btVector3 objBOrigin = m_objectB->GetPhysicsComponent()->GetPhysicsBody()->getWorldTransform().getOrigin()* btVector3(1, 0, 1);

	float distance = (parentOrigin - objBOrigin).length();
	float triggerSquare = (m_triggerDistance * m_triggerDistance);
	if (m_triggerWhenLessThan) {
		if ((m_parent->GetPhysicsComponent()->GetPhysicsBody()->getWorldTransform().getOrigin() - m_objectB->GetPhysicsComponent()->GetPhysicsBody()->getWorldTransform().getOrigin()).length2() <= (m_triggerDistance * m_triggerDistance)) {
			std::cout << "Shoot -> position \t\t distance =  " << distance << "\t trigger = " << m_triggerDistance << std::endl;
			return true;
		}
	}
	else {
		if ((m_parent->GetPhysicsComponent()->GetPhysicsBody()->getWorldTransform().getOrigin() - m_objectB->GetPhysicsComponent()->GetPhysicsBody()->getWorldTransform().getOrigin()).length2() >= (m_triggerDistance * m_triggerDistance)) {
			std::cout << "Shoot -> position \t\t distance =  " << distance << "\t trigger = " << m_triggerDistance << std::endl;
			return true;
		}
	}
	return false;
	
}
