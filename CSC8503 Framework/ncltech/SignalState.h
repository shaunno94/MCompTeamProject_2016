#pragma once
#include "State.h"
#include <vector>
#include <string>

class DataSourceState;

class SignalState : public State {
protected:
	DataSourceState* m_DataSource;
 public:
	SignalState(GameObject& go);
	~SignalState(void);

	std::vector<std::string> m_Matches;

	void Start(State* previousState) override;

	void Update(float deltaTime) override;

	void AddMatch(std::string& str);
};

