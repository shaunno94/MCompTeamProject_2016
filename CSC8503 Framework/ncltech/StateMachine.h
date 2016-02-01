#pragma once

#include "State.h"
#include "StateTransition.h"
#include <vector>

class StateMachine
{
	State* m_CurrentState;
	unsigned int m_CurrentStateIndex; 
public:

	std::vector<State*> States;
	std::vector<std::vector<std::pair<StateTransition*, unsigned int>>> StateTransitions;

	StateMachine(void);
	~StateMachine(void);

	void AddState(State* state, std::vector<std::pair<StateTransition*, unsigned int>>& transitions);

	void Update(float deltaTime);

	State* GetCurrentState();
	unsigned int GetCurrentStateIndex();
};

