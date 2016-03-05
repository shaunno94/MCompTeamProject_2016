#pragma once

#include "Networking/Net.h"
#include "ControllerComponent.h"

struct NetMessageDataPhysicsState
{
	Vec3Physics position;
	QuatPhysics orientation;
	Vec3Physics linearVelocity;
	Vec3Physics angularVelocity;
};

//TODO: maybe store controllerComponent once it's clear if you might need a special NetControllerComponent
class PCNetController
{
public:
	PCNetController(ControllerComponent* component, unsigned int id);
	virtual ~PCNetController();

	virtual void Sync(NetSessionReader& reader);
	virtual void Sync(NetSessionWriter& writer);

protected:
	ControllerComponent* m_component;
	int m_id;
};

