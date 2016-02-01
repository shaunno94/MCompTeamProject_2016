#include "StateTransition.h"

#include "GameObject.h"

StateTransition::StateTransition(State* oldState, State* newState) {
	m_OldState = oldState;
	m_NewState = newState;
	m_Active = false;
}


StateTransition::~StateTransition(void) {
}

void* StateTransition::empty = nullptr;

void StateTransition::Start() { m_Active = true; }

bool StateTransition::Transition() { return true; }

void StateTransition::End() { m_Active = false; }