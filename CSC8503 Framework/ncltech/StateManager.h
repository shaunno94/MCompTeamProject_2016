#pragma once
#include <map>

class State

class StateManager
{
public:
	StateManager();
	~StateManager();

	void AddState(std::string stateName, State* state);

	State* GetState(std::string stateName);

protected:
	std::map<std::string, State*>* m_stateMap;

	State* findState(std::string stateName);
};

