#pragma once
#include "PickupGameObject.h"

class BoostPickupCollisionFilterStep : public ParticleFilterStep
{
public:
	virtual bool callback(btBroadphaseProxy* proxy0, btBroadphaseProxy* proxy1) const override;

	BoostPickupCollisionFilterStep(GameObject* obj);

protected:
	GameObject* m_obj;
	int m_broadphaseProxyId;
};


class BoostPickupGameObject : public PickupGameObject
{
public:
	BoostPickupGameObject(Material* mat, Mesh* mesh, const Vec3Physics& pos = Vec3Physics::ZEROS);
	~BoostPickupGameObject();

};

