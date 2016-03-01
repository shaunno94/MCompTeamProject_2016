#include "State.h"
#include "StateMachine.h"


State::State(StateMachine& stateMachine, GameObject& parent) :
m_stateMachine(&stateMachine),
m_parent(&parent),
m_childStates(new stateMapping())
{

}


State::~State()
{
	if (m_childStates->size() != 0) {
		for (auto it = m_childStates->begin();
			it != m_childStates->end();
			it++)
		{
			delete it->second;
			m_childStates->erase(it);
		}
	}
	delete m_childStates;

	if (m_triggers.size() != 0) {
		for (triggerPair* pair : m_triggers) {
			delete pair->first;
		}
	}

}

void State::Update(float dt)
{	
	if (m_childStates->size() != 0)
	{
		(*m_childStates)[&m_activeChildState]->Update(dt);
	}
	CheckTriggers();
}

bool State::CheckTriggers()
{
	for (triggerPair* &trigger : m_triggers) {
		if (trigger->first->HasTriggered()) {
			m_stateMachine->ChangeState(trigger->second);
			return true;
		}
	}

	return false;
}

void State::AddChildState(unsigned int stateName, State* childState)
{
	if (m_childStates->find(&stateName) == m_childStates->end())
		(*m_childStates)[&stateName] = childState;
}

void State::AddTrigger(Trigger* trigger , unsigned int destState)
{
	m_triggers.push_back(new triggerPair(trigger, destState));
}

