#pragma once
#include <vector>

using namespace std;

class GameObject;

class State
{
protected:
	GameObject* m_GameObject;
	bool m_Active;

	// Is this a hierarchical FSM in which this state has sub-states?
	vector<State*> m_ChildStates;

	// If so, which of these child states are active
	int m_activeState;

public:
	State(string name, GameObject& go);
	virtual ~State(void);

	virtual void Start();

	virtual void Update(float deltaTime);

	virtual void End();

	// Helper variable for debugging purposes
	string stateName;

};

