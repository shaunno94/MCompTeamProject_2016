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

}

void State::AddChildState(State* childState)
{
	m_childStates.push_back(childState);
}

void State::AddTrigger(Trigger* trigger , unsigned int destState)
{
	m_triggers.push_back(new std::pair<Trigger*, unsigned int>(trigger, destState));
}

