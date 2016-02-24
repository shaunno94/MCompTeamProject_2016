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
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_UP) || Window::GetKeyboard()->KeyDown(KEYBOARD_W))
	{
		force += (orientation * Vec3Physics(5, 0, 0));
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_DOWN) || Window::GetKeyboard()->KeyDown(KEYBOARD_S))
	{
		force += (orientation * Vec3Physics(-5, 0, 0));
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_RIGHT) || Window::GetKeyboard()->KeyDown(KEYBOARD_D))
	{
		//torque += (Vec3Physics(0,1000, 0));
		force += (orientation * Vec3Physics(0, 0, 5));
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_LEFT) || Window::GetKeyboard()->KeyDown(KEYBOARD_A))
	{
		//torque += (Vec3Physics(0, -1000, 0));
		force += (orientation * Vec3Physics(0, 0, -5));
	}
	object->AddForce(force.x, force.y, force.z);
	object->AddTorque(torque.x, torque.y, torque.z);

	float pitch = (Window::GetMouse()->GetRelativePosition().y);
	float yaw = (Window::GetMouse()->GetRelativePosition().x);
	object->setCameraControl(pitch, yaw);
}
