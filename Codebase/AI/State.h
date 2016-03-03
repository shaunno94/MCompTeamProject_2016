#pragma once
#include <vector>
#include <utility>
#include <map>
#include "Trigger.h"

class StateMachine;

class State
{
public:
	State(StateMachine& stateMachine, GameObject& parent);
	~State();

	virtual void Start() = 0;
	virtual void Update(float dt);

	void AddChildState(unsigned int stateName, State* childState);
	void AddTrigger(Trigger* trigger, unsigned int destState);

protected:

	bool CheckTriggers();

	typedef std::pair<Trigger*, unsigned int> triggerPair;
	typedef std::map<unsigned int, State*> stateMapping;

	stateMapping* m_childStates;
	unsigned int m_activeChildState;

	std::vector<triggerPair*> m_triggers;

	GameObject* m_parent;
	StateMachine* m_stateMachine;
};