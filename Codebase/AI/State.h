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

	void AddChildState(const std::string& stateName, State* childState);
	void AddTrigger(Trigger* trigger, const std::string& destState);
	
protected:

	bool CheckTriggers();

	typedef std::pair<Trigger*, std::string> triggerPair;
	typedef std::map<const std::string*, State*> stateMapping;

	stateMapping* m_childStates;
	std::string	m_activeChildState;

	std::vector<triggerPair*> m_triggers;

	GameObject* m_parent;
	StateMachine* m_stateMachine;
};

