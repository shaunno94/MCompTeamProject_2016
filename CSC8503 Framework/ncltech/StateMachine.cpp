#include "StateMachine.h"


StateMachine::StateMachine()
{
	m_stateMap = new stateMapping();
}


StateMachine::~StateMachine()
{

	for (stateMapping::iterator it = m_stateMap->begin();
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
void StateMachine::AddState(std::string stateName, State* state)
{
	if (m_stateMap->find(stateName) == m_stateMap->end())
		(*m_stateMap)[stateName] = state;
}

/// <summary>
///		Get specific State
/// </summary>
/// <param name="stateName"> Name of state to change to </param>
/// <returns> State matching </returns>
bool StateMachine::ChangeState(std::string stateName)
{
	if (m_stateMap->find(stateName) != m_stateMap->end()) {
		activeState = (*m_stateMap)[stateName];
		activeState->Start();
		return true;
	}
	return false;
}

