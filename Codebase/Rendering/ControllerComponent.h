#pragma once
#include "Math\nclglMath.h"
class GameObject;

class ControllerComponent
{
public:
	ControllerComponent(GameObject* parent);
	~ControllerComponent();
	void updateObject(float dt);
	void AddForce(float x, float y, float z);
	void AddTorque(float x, float y, float z);

	void setCameraControl(float pitch, float yaw);
	void getCameraControl(float& pitch, float& yaw);

	Mat4Physics getOrientation();
protected:
	GameObject* m_parent;
	Vector3Simple force;
	Vector3Simple torque;
	float dPitch, dYaw;
};

