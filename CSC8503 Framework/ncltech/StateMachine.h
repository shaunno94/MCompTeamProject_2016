#pragma once

#include "State.h"
#include "StateTransition.h"
#include <vector>

class StateMachine
{
	State* m_CurrentState;
	unsigned int m_CurrentStateIndex; 
public:

	std::vector<State*> States;		// Vector of states in this FSM
	std::vector<std::vector<std::pair<StateTransition*, unsigned int>>> StateTransitions;	// Transitions: 1- transition, 2- index of dest state in state vector

	StateMachine(void);
	~StateMachine(void);

	void AddState(State* state, std::vector<std::pair<StateTransition*, unsigned int>>& transitions);

	void Update(float deltaTime);

	State* GetCurrentState();
	unsigned int GetCurrentStateIndex();
};

