#pragma once

#include "StateTransition.h"

class ReceiverTransition : public StateTransition
{
public:
	ReceiverTransition(State* oldState, State* newState);
	~ReceiverTransition(void);

	bool Transition() override;
};

