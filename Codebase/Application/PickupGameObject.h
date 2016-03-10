#pragma once
#include "Rendering/GameObject.h"

#include "PhysicsEngine/PhysicsEngineInstance.h"
#include "Helpers/DeltaTimer.h"

#define PICKUP_INVISABILITY_TIMOUT 7000

class PickupGameObject : public GameObject
{
public:
	PickupGameObject(const std::string& name, float radius, const Vec3Physics& pos = Vec3Physics::ZEROS);
	~PickupGameObject();

	virtual void OnUpdateObject(float dt) override;

	inline int GetBroadphaseProxyId()
	{
		return m_proxyId;
	}

	void Disable(float millisec);
	void Enable();

protected:
	int m_proxyId;
	DeltaTimer<float> m_timer;
	bool m_disabled;
	btTransform m_lastActiveTransform;
	float m_timeoutMillisec;
	Vec3Physics m_initialPos;
};

