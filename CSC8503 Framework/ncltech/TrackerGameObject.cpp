#include "TrackerGameObject.h"
#include "SphereCollisionShape.h"

TrackerGameObject::TrackerGameObject(PhysicsObject* target, float radius) : GameObject("Tracker") {
	m_Target = target;
	SetBoundingRadius(radius);
	Physics()->NeverRest(true);
	Physics()->SetCollisionShape(new SphereCollisionShape(radius));

	Physics()->SetOnCollisionCallback(
	[&](PhysicsObject * obj) {
		if (obj != m_Target && !obj->AtRest()) {
			for (CollisionListener* listener : m_Listeners)
				listener->callback(obj);
		}
		return false;
	}
	);
	Physics()->SetPosition(m_Target->GetPosition());
}


TrackerGameObject::~TrackerGameObject(void) {
}

void TrackerGameObject::AddCollisionListener(CollisionListener* listener) {
	m_Listeners.push_back(listener);
}