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
	Mat4Physics orientation; //= object->getOrientation();
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_UP))
	{
		force += (orientation * Vec3Physics(5, 0, 0));
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_DOWN))
	{
		force += (orientation * Vec3Physics(-5, 0, 0));
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_RIGHT))
	{
		//torque += (Vec3Physics(0,1000, 0));
		force += (orientation * Vec3Physics(0, 0, 5));
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_LEFT))
	{
		//torque += (Vec3Physics(0, 0, -1000));
		force += (orientation * Vec3Physics(0, 0, -5));
	}
	object->AddForce(force.x, force.y, force.z);
	object->AddTorque(torque.x, torque.y, torque.z);
}
