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
	Vec3Physics impulse(0, 0, 0);
	Mat4Physics orientation = object->getOrientation();
	float turn = 0;
	bool airbourne = object->airbourne();

	float forward = object->getForwardVelocity();

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_UP) || Window::GetKeyboard()->KeyDown(KEYBOARD_W))
	{
		force += (orientation * Vec3Physics(0, 0, -1)).Normalize()*accel;
		if (airbourne)
			torque += (orientation *Vec3Physics(-1, 0, 0)).Normalize() * airAccel;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_DOWN) || Window::GetKeyboard()->KeyDown(KEYBOARD_S))
	{
		force += (orientation * Vec3Physics(-0, 0, 1)).Normalize() * accel;
		if (airbourne)
			torque += (orientation *Vec3Physics(1, 0, 0)).Normalize() * airAccel;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_RIGHT) || Window::GetKeyboard()->KeyDown(KEYBOARD_D))
	{

		if (airbourne)
			torque += (orientation *Vec3Physics(0, 0, -1)).Normalize() * airAccel * 0.7f;
		else
		if (forward < 0){
			torque += (orientation *Vec3Physics(0, -1, 0)).Normalize() * -rotAccel;
		}
		else{
			torque += (orientation *Vec3Physics(0, -1, 0)).Normalize() * rotAccel;
		}

		turn++;
		//force += (orientation * Vec3Physics(0, 0, accel));
	}

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_LEFT) || Window::GetKeyboard()->KeyDown(KEYBOARD_A))
	{
		if (airbourne)
			torque += (orientation *Vec3Physics(0, 0, 1)).Normalize() * airAccel * 0.7f;
		else
		if (forward < 0){
			torque += (orientation *Vec3Physics(0, 1, 0)).Normalize() * -rotAccel;
		}
		else{
			torque += (orientation *Vec3Physics(0, 1, 0)).Normalize() * rotAccel;
		}

		turn--;
		//force += (orientation * Vec3Physics(0, 0, -accel));
	}

	object->turnWheels(turn);

	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_SPACE))
	{
		impulse += (orientation * Vec3Physics(0, jump, -jump *.5));
	}

	object->AddForce(force.x, force.y, force.z);
	object->AddTorque(torque.x, torque.y, torque.z);
	object->AddImpulse(impulse.x, impulse.y, impulse.z);

	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_R))
	{
		object->reset();
	}

	//------camera control-------//
	float pitch = (Window::GetMouse()->GetRelativePosition().y);
	float yaw = (Window::GetMouse()->GetRelativePosition().x);
	object->setCameraControl(pitch, yaw);
}
#endif