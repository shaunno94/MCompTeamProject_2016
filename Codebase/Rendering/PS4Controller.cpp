#ifdef ORBIS
#include "PS4Controller.h"


PS4Controller::PS4Controller(ControllerComponent* object)
{
	setObject(object);
}


PS4Controller::~PS4Controller()
{
}

void PS4Controller::CheckInput(){
	Vec3Physics force(0, 0, 0);
	Vec3Physics torque(0, 0, 0);
	float accel = 7;
	Mat4Physics orientation = object->getOrientation();

	force = (orientation * Vec3Physics(0, 0, PS4Input::getPS4Input()->GetAxis(LEFTSTICK).x)).Normalize()*accel;
	
	torque = (Vec3Physics(0, -PS4Input::getPS4Input()->GetAxis(LEFTSTICK).y * 3, 0));	
	
	object->turnWheels(PS4Input::getPS4Input()->GetAxis(LEFTSTICK).y);

	if (PS4Input::getPS4Input()->GetButtonDown(BTN_CROSS))
	{
		//torque += (orientation * Vec3Physics(0, 5, 0));
	}

	object->AddForce(force.x, force.y, force.z);
	object->AddTorque(torque.x, torque.y, torque.z);

	if (PS4Input::getPS4Input()->GetButtonDown(BTN_R3))
	{
		object->reset();
	}
	float pitch = (PS4Input::getPS4Input()->GetAxis(RIGHTSTICK).y);
	float yaw = (PS4Input::getPS4Input()->GetAxis(RIGHTSTICK).x);
	object->setCameraControl(pitch, yaw);
}
#endif