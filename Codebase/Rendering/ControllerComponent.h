#pragma once
#include "Math\nclglMath.h"
class GameObject;

class ControllerComponent
{
public:
	ControllerComponent(GameObject* parent);
	~ControllerComponent();
	virtual void updateObject(float dt);
	virtual void AddForce(float x, float y, float z);
	virtual void AddTorque(float x, float y, float z);
	virtual void AddImpulse(float x, float y, float z);
	void turnWheels(float prop);
	bool airbourne();

	void SetState(const Vec3Physics& pos, const QuatPhysics& orientation, const Vec3Physics& linearVelocity, const Vec3Physics& angularVelocity);

	void setCameraControl(float pitch, float yaw);
	void getCameraControl(float& pitch, float& yaw);

	Mat4Physics getOrientation();
	float getForwardVelocity();
	void reset();

	GameObject* GetParent()
	{
		return m_parent;
	}

	bool adjustForRotation = false;
protected:
	GameObject* m_parent;
	Vec3Physics force;
	Vec3Physics torque;
	Vec3Physics impulse;
	float dPitch, dYaw;

	bool m_updateState;
	QuatPhysics m_orientation;
	Vec3Physics m_position;
	Vec3Physics m_linearVelocity;
	Vec3Physics m_angularVelocity;

	unsigned int m_inactiveFramesUpsideDown;
};

