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

	void setCameraControl(float pitch, float yaw);
	void getCameraControl(float& pitch, float& yaw);

	Mat4Physics getOrientation();
	void reset();
protected:
	GameObject* m_parent;
	Vector3Simple force;
	Vector3Simple torque;
	float dPitch, dYaw;
};

