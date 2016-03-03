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

	void setCameraControl(float pitch, float yaw);
	void getCameraControl(float& pitch, float& yaw);

	Mat4Physics getOrientation();
	float getForwardVelocity();
	void reset();
protected:
	GameObject* m_parent;
	Vector3Simple force;
	Vector3Simple torque;
	Vector3Simple impulse;
	float dPitch, dYaw;

	unsigned int m_inactiveFramesUpsideDown;
};

