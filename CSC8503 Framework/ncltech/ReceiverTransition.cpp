#include "ReceiverTransition.h"

#include "ReceiverState.h"
#include <stdexcept>

ReceiverTransition::ReceiverTransition(State* oldState, State* newState) : StateTransition(oldState, newState) {
	if (!dynamic_cast<ReceiverState*>(oldState))
		std::invalid_argument("oldState is not an instance of ReceiverState");
}


ReceiverTransition::~ReceiverTransition(void) {
}

bool ReceiverTransition::Transition() {
	return reinterpret_cast<ReceiverState*>(m_OldState)->GetData();
}