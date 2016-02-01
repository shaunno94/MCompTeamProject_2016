#include "StateMachine.h"


StateMachine::StateMachine(void) {
	m_CurrentState = nullptr;
	m_CurrentStateIndex = 0;
}


StateMachine::~StateMachine(void) {
	for (State* state : States)
		delete state;
	States.clear();
	for (std::vector<std::pair<StateTransition*, unsigned int>>& transitionSet : StateTransitions) {
		for (std::pair<StateTransition*, unsigned int>& transition : transitionSet)
			delete transition.first;
		transitionSet.clear();
	}
	StateTransitions.clear();
}


void StateMachine::AddState(State* state, std::vector<std::pair<StateTransition*, unsigned int>>& transitions) {
	States.push_back(state);
	StateTransitions.push_back(transitions);
}

void StateMachine::Update(float deltaTime) {
	if(!m_CurrentState) {
		m_CurrentState = States[m_CurrentStateIndex];
		m_CurrentState->Start(nullptr);
	}
	m_CurrentState->Update(deltaTime);
	for (std::pair<StateTransition*, unsigned int> transition : StateTransitions[m_CurrentStateIndex])
	{
		if(transition.first->Transition()) {
			m_CurrentState->End();
			m_CurrentStateIndex = transition.second;
			State* oldState = m_CurrentState;
			m_CurrentState = States[m_CurrentStateIndex];
			m_CurrentState->Start(oldState);
			return;
		}
	}
}