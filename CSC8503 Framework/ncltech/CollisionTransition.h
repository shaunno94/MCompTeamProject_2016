#pragma once

#include "StateTransition.h"
#include <string>
#include <vector>
#include <functional>

class GaneObject;
class PhysicsObject;

class CollisionTransition : public StateTransition
{
private:

public:
	CollisionTransition(State* oldState, State* newState);
	~CollisionTransition(void);

	virtual bool Transition() override;

};

