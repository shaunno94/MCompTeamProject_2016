#include "StateMachine.h"
#include <iostream>

StateMachine::StateMachine()
{
	m_stateMap = new stateMapping();
}

StateMachine::~StateMachine()
{

	for (auto it = m_stateMap->begin();
		it != m_stateMap->end();
		it++)
	{
		delete it->second;
	}

	m_stateMap->clear();
	delete m_stateMap;
	m_stateMap = nullptr;
	activeState = nullptr;
}

void StateMachine::Update(float dt)
{
	activeState->Update(dt);
}

/// <summary>
///		Check if state has already been added to the StateManager and if not load in
/// </summary>
/// <param name="stateName"> Identifier for State </param>
/// <param name="state"> Actual State object</param>
void StateMachine::AddState(unsigned int stateName, State* state)
{
	if (m_stateMap->find(stateName) == m_stateMap->end())
		(*m_stateMap)[stateName] = state;
}

/// <summary>
///		Get specific State
/// </summary>
/// <param name="stateName"> Name of state to change to </param>
/// <returns> State matching </returns>
bool StateMachine::ChangeState(unsigned int stateName)
{
	if (m_stateMap->find(stateName) != m_stateMap->end()) {
		activeState = (*m_stateMap)[stateName];
		activeState->Start();
		return true;
	}
	else {
		activeState->ChangeChildState(stateName);
		return true;
	}
	std::cout << "Could not find state " << stateName << " in state machine" << std::endl;
	return false;
}