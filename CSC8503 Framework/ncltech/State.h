#pragma once
#include <vector>
#include "Trigger.h"

class State
{
public:
	State();
	~State();

	virtual void Update(float dt);
	void CheckTriggers();

	void AddChildState(State* childState);
	void AddTrigger(Trigger* trigger, unsigned int destState);
	
protected:
	typedef std::pair<Trigger, unsigned int> triggerPair;

	std::vector<State*> m_childStates;
	int m_activeChildState; // = 0 if no children

	std::vector<triggerPair*> m_triggers;
};

