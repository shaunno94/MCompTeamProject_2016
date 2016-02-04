#include "State.h"


State::State()
{
	m_activeChildState = 0;
}


State::~State()
{
}

void State::Update(float dt)
{
	if (m_activeChildState != 0)
	{
		m_childStates[m_activeChildState]->Update(dt);
	}
	CheckTriggers();
}

bool State::CheckTriggers()
{
	for (triggerPair* &trigger : m_triggers) {
		if (trigger->first->HasTriggered()) {
			m_activeChildState = trigger->second;
			return true;
		}
	}

	return false;
}

void State::AddChildState(State* childState)
{
	m_childStates.push_back(childState);
}

void State::AddTrigger(Trigger* trigger , unsigned int destState)
{
	m_triggers.push_back(new triggerPair(trigger, destState));
}

