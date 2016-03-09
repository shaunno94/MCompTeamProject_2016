#pragma once
#include "PhysicsEngine/PhysicsEngineInstance.h"

class GoalCollisionFilterStep : public ParticleFilterStep
{
public:
	virtual bool callback(btBroadphaseProxy* proxy0, btBroadphaseProxy* proxy1) const override;

	GoalCollisionFilterStep();
};

