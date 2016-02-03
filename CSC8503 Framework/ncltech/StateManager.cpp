#include "StateManager.h"
#include "State.h"

StateManager::StateManager()
{
	m_stateMap = new std::map<std::string, State*>();
}

StateManager::~StateManager()
{
}

/// <summary>
///		Check if state has already been added to the StateManager and if not load in
/// </summary>
/// <param name="stateName"> Identifier for State </param>
/// <param name="state"> Actual State object</param>
void StateManager::AddState(std::string stateName, State* state)
{
	if (findState(stateName) != nullptr)
		m_stateMap->insert(m_stateMap->end(), std::pair<std::string, State*>(stateName, state));
}

/// <summary>
///		Get specific State
/// </summary>
/// <param name="stateName"></param>
/// <returns> State matching </returns>
State* StateManager::GetState(std::string stateName)
{
	return findState(stateName);
}

State* StateManager::findState(std::string stateName)
{
	std::map<std::string, State*>::iterator it = m_stateMap->begin;
	for (it;
	     it != m_stateMap->end;
	     it++)
	{
		if (it->first == stateName)
		{
			return it->second;
		}
	}

	return nullptr;
}
