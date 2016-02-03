#pragma once
class State;

class StateTransition
{
protected:
	State* m_OldState;
	State* m_NewState;

public:
	StateTransition(State* oldState, State* newState);

	virtual ~StateTransition(void);

	// Does this transition require something to be set up?
	virtual void Start() = 0;

	// Check the trigger to see if transition is required
	virtual bool CheckTrigger() = 0;

	// Clean up
	virtual void End() = 0;
};

