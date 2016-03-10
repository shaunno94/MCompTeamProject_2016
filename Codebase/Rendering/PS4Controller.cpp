#ifdef ORBIS
#include "PS4Controller.h"
#include "Renderer.h"


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
		torque += (orientation *Vec3Physics(0, 0, -1)).Normalize() * PS4Input::getPS4Input()->GetAxis(LEFTSTICK).x * airAccel * 0.7f;
		torque += (orientation *Vec3Physics(1, 0, 0)).Normalize() * PS4Input::getPS4Input()->GetAxis(LEFTSTICK).y * airAccel;
	}
	else
	{
		force = (orientation * Vec3Physics(0, 0, -1)).Normalize() * PS4Input::getPS4Input()->GetButton(BTN_R2)* accel;
		force += (orientation * Vec3Physics(0, 0, 1)).Normalize() * PS4Input::getPS4Input()->GetButton(BTN_L2)* accel;
		torque = (Vec3Physics(0, -PS4Input::getPS4Input()->GetAxis(LEFTSTICK).y * rotAccel, 0));

		torque = (Vec3Physics(0, -PS4Input::getPS4Input()->GetAxis(LEFTSTICK).x * rotAccel, 0));
		if (forward < 0){
			torque = -torque;
		}
		object->turnWheels(PS4Input::getPS4Input()->GetAxis(LEFTSTICK).x);

		if (PS4Input::getPS4Input()->GetButtonDown(BTN_CROSS))
		{
			impulse += (orientation * Vec3Physics(0, jump, -jump *.5));
		}
	}

	if (object->boost > 0 && PS4Input::getPS4Input()->GetButtonDown(BTN_CIRCLE))
	{
		force *= 2;
		torque *= 2;
		object->boost -= 0.01;
	}

	object->AddForce(force.x, force.y, force.z);
	object->AddTorque(torque.x, torque.y, torque.z);
	object->AddImpulse(impulse.x, impulse.y, impulse.z);

	if (PS4Input::getPS4Input()->GetButtonTriggered(BTN_TRIANGLE))
	{
		object->reset();
	}

	if (PS4Input::getPS4Input()->GetButtonTriggered(BTN_R3))
	{
		Renderer::GetInstance()->GetCurrentScene()->getCamera()->reset();
	}

	if (PS4Input::getPS4Input()->GetButtonTriggered(BTN_R1))
	{
		Renderer::GetInstance()->GetCurrentScene()->getCamera()->autocam = !Renderer::GetInstance()->GetCurrentScene()->getCamera()->autocam;
		Renderer::GetInstance()->GetCurrentScene()->getCamera()->reset();
	}

	float sensitivityY = 1;
	float sensitivityX = 3;
	float pitch = (PS4Input::getPS4Input()->GetAxis(RIGHTSTICK).y * sensitivityY);
	float yaw = (PS4Input::getPS4Input()->GetAxis(RIGHTSTICK).x * sensitivityX);
	object->setCameraControl(pitch, yaw);
}
#endif