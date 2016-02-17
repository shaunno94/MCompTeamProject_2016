#pragma once
#include <map>
#include "State.h"

class StateMachine
{
public:
	StateMachine();
	~StateMachine();

	// Call update function on active state
	void Update(float dt);

	// Add new State to the state machine
	void AddState(std::string stateName, State* state);
	// Change currently active state - true if success
	bool ChangeState(std::string stateName);

	State* GetCurrentState()
	{
		return activeState;
	}

protected:

	typedef std::map<std::string, State*> stateMapping;
	stateMapping* m_stateMap;
	State* activeState;

};

