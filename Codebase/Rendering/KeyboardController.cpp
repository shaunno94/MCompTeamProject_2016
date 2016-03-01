#ifndef ORBIS
#include "KeyboardController.h"


KeyboardController::KeyboardController(ControllerComponent* object)
{
	setObject(object);
}


KeyboardController::~KeyboardController()
{
}

void KeyboardController::CheckInput(){
	Vec3Physics force(0, 0, 0);
	Vec3Physics torque(0, 0, 0);
	float accel = 7;
	Mat4Physics orientation = object->getOrientation();
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_UP) || Window::GetKeyboard()->KeyDown(KEYBOARD_W))
	{
		force += (orientation * Vec3Physics(0, 0, -1)).Normalize()*accel;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_DOWN) || Window::GetKeyboard()->KeyDown(KEYBOARD_S))
	{
		force += (orientation * Vec3Physics(-0, 0, 1)).Normalize() * accel;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_RIGHT) || Window::GetKeyboard()->KeyDown(KEYBOARD_D))
	{
		torque += (Vec3Physics(0,-5, 0));
		//force += (orientation * Vec3Physics(0, 0, accel));
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_LEFT) || Window::GetKeyboard()->KeyDown(KEYBOARD_A))
	{
		torque += (Vec3Physics(0, 5, 0));
		//force += (orientation * Vec3Physics(0, 0, -accel));
	}

	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_SPACE))
	{
		//torque += (orientation * Vec3Physics(0, 5, 0));
	}

	object->AddForce(force.x, force.y, force.z);
	object->AddTorque(torque.x, torque.y, torque.z);

	float pitch = (Window::GetMouse()->GetRelativePosition().y);
	float yaw = (Window::GetMouse()->GetRelativePosition().x);
	object->setCameraControl(pitch, yaw);

	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_R))
	{
		object->reset();
	}
}
#endif