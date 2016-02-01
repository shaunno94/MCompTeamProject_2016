#pragma once

#include "State.h"

class DataSourceState : public State
{
public:
	DataSourceState(GameObject& go);

	virtual void* GetData() = 0;
};

