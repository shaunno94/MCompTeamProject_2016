#include "Trigger.h"
#include "Rendering/GameObject.h"


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
