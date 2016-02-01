#pragma once
#include "ReceiverState.h"
#include "Math/nclglMath.h"
#include <string>
#include <vector>
#include <functional>

class PhysicsObject;
class GameObject;

class PositioningState : public State
{
protected:
	Vec3Physics m_Displacement;
	QuatPhysics m_Rotation;
	PhysicsObject* m_Physics;
	float m_DeltaTime;

	Mat4Graphics m_LocalTransformCopy;

public:
	PositioningState(GameObject& go, const Vec3Physics& displacement, const QuatPhysics& rotation);
	~PositioningState(void);

	virtual void Update(float deltaTime) override;
};

