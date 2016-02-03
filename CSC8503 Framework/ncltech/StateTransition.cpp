#include "StateTransition.h"

StateTransition::StateTransition(State* oldState, State* newState) {
	m_OldState = oldState;
	m_NewState = newState;
}