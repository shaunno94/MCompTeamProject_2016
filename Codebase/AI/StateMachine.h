#pragma once
#include <map>
#include "State.h"

namespace stringWrapper
{
	typedef std::string sWrapper;
}
using namespace stringWrapper;
class StateMachine
{
public:
	StateMachine();
	~StateMachine();

	// Call update function on active state
	void Update(float dt);

	// Add new State to the state machine
	void AddState(const std::string& stateName, State* state);
	// Change currently active state - true if success
	bool ChangeState(const std::string& stateName);

	State* GetCurrentState()
	{
		return activeState;
	}

protected:

	typedef std::map<const std::string* , State*> stateMapping;
	stateMapping* m_stateMap;
	State* activeState;

};

