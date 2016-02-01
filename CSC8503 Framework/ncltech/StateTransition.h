#pragma once

class GameObject;
class State;

class StateTransition
{
protected:
	static void* empty;
	bool m_Active;
	State* m_OldState;
	State* m_NewState;

public:
	StateTransition(State* oldState, State* newState);

	virtual ~StateTransition(void);

	virtual void Start();

	virtual bool Transition();

	virtual void End();
};

