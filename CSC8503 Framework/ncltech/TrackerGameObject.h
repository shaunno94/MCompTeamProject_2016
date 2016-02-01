#pragma once
#include "GameObject.h"
#include <mutex>
#include "CollisionListener.h"

class TrackerGameObject : public GameObject
{
protected:
	PhysicsObject* m_Target;
	std::vector<CollisionListener*> m_Listeners;

public:
	TrackerGameObject(PhysicsObject* target, float radius = 1.0f);
	~TrackerGameObject(void);

	void AddCollisionListener(CollisionListener* listener);
};

