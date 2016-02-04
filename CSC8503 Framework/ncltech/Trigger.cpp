#include "Trigger.h"


Trigger::Trigger()
{
}


Trigger::~Trigger()
{
}

void Trigger::setupTrigger(GameObject& parentObject)
{
	m_parent = &parentObject;
}
