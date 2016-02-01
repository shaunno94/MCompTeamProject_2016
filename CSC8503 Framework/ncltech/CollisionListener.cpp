#include "CollisionListener.h"


CollisionListener::CollisionListener(void)
{
	callback = [](PhysicsObject*){};
}


CollisionListener::~CollisionListener(void)
{
}
