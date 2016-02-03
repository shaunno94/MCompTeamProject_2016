#include "StateMachine.h"


StateMachine::StateMachine(void) {
	m_CurrentState = nullptr;
	m_CurrentStateIndex = 0;
}


StateMachine::~StateMachine(void) {

	// Delete States
	for (State* state : States)
		delete state;
	States.clear();

	// Delete Transitions
	for (std::vector<std::pair<StateTransition*, unsigned int>>& transitionSet : StateTransitions) {
		for (std::pair<StateTransition*, unsigned int>& transition : transitionSet)
			delete transition.first;
		transitionSet.clear();
	}
	StateTransitions.clear();
}

/// <summary>
/// Add a state to the state machine
/// </summary>
/// <param name="state"> The new state</param>
/// <param name="transitions"> Transition-integer pairs defining the transitions associated with this new state, and
///		connecting them to the index of the destination state </param>
void StateMachine::AddState(State* state, std::vector<std::pair<StateTransition*, unsigned int>>& transitions) {
	States.push_back(state);
	StateTransitions.push_back(transitions);
}

/// <summary>
/// Calls the update function on the currently active state in the FSM
/// </summary>
/// <param name="deltaTime"> Time since last frame </param>
void StateMachine::Update(float deltaTime) {

	// If no state has been assigned, take state from available states and start it
	if(!m_CurrentState) {
		m_CurrentState = States[m_CurrentStateIndex];
		m_CurrentState->Start();
	}

	// Call appropriate update function on the gameobject
	m_CurrentState->Update(deltaTime);

	// Iterate through each transition pair, and if a transition is triggered, make the change to the new state
	//		using the index held in the pair. Also start this new state.
	for (std::pair<StateTransition*, unsigned int> transition : StateTransitions[m_CurrentStateIndex])
	{
		if(transition.first->Transition()) {
			m_CurrentState->End();
			m_CurrentStateIndex = transition.second;
			State* oldState = m_CurrentState;
			m_CurrentState = States[m_CurrentStateIndex];
			m_CurrentState->Start();
			return;
		}
	}
}