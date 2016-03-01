#pragma once
#include "State.h"
#include <unordered_map>

class StateMachine
{
public:
	StateMachine();
	~StateMachine();

	// Call update function on active state
	void Update(float dt);

	// Add new State to the state machine
	void AddState(const std::string& stateName, State* state);
	// Change currently active state - true if success
	bool ChangeState(const std::string& stateName);

	State* GetCurrentState()
	{
		return activeState;
	}

protected:

	typedef std::unordered_map<unsigned int, State*> stateMapping;
	stateMapping* m_stateMap;
	State* activeState;

};

