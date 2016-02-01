#pragma once

#include "PhysicsObject.h"
#include <functional>

class CollisionListener
{
public:
	CollisionListener(void);
	~CollisionListener(void);

	std::function<void(PhysicsObject*)> callback;
};

