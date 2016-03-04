#ifdef ORBIS
#include "PS4Controller.h"


PS4Controller::PS4Controller(ControllerComponent* object)
{
	setObject(object);
}


PS4Controller::~PS4Controller()
{
}

void PS4Controller::CheckInput()
{
	Vec3Physics force(0, 0, 0);
	Vec3Physics torque(0, 0, 0);
	Vec3Physics impulse(0, 0, 0);
	Mat4Physics orientation = object->getOrientation();
	float forward = object->getForwardVelocity();

	if (object->airbourne())
	{
		torque += (orientation *Vec3Physics(0, 0, -PS4Input::getPS4Input()->GetAxis(LEFTSTICK).y)).Normalize() * airAccel * 0.7f;
		torque += (orientation *Vec3Physics(-PS4Input::getPS4Input()->GetAxis(LEFTSTICK).x, 0, 0)).Normalize() * airAccel;
	}
	else
	{
		force = (orientation * Vec3Physics(0, 0, -PS4Input::getPS4Input()->GetButton(BTN_R2))).Normalize() * accel;
		force += (orientation * Vec3Physics(0, 0, PS4Input::getPS4Input()->GetButton(BTN_L2))).Normalize() * accel;
		torque = (Vec3Physics(0, -PS4Input::getPS4Input()->GetAxis(LEFTSTICK).x * rotAccel, 0));

	object->turnWheels(PS4Input::getPS4Input()->GetAxis(LEFTSTICK).x);

	if (PS4Input::getPS4Input()->GetButtonDown(BTN_CROSS))
	{
			impulse += (orientation * Vec3Physics(0, jump, -jump *.5));
		}
	}

	object->AddForce(force.x, force.y, force.z);
	object->AddTorque(torque.x, torque.y, torque.z);
	object->AddImpulse(impulse.x, impulse.y, impulse.z);

	if (PS4Input::getPS4Input()->GetButtonDown(BTN_TRIANGLE))
	{
		object->reset();
	}
	float pitch = (PS4Input::getPS4Input()->GetAxis(RIGHTSTICK).y);
	float yaw = (PS4Input::getPS4Input()->GetAxis(RIGHTSTICK).x);
	object->setCameraControl(pitch, yaw);
}
#endif