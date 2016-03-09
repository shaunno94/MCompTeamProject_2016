#include "InputBase.h"
#include <math.h>


InputBase::InputBase()
{
	ResetInput();
}


InputBase::~InputBase()
{
}

void InputBase::ResetInput() {
	for (int i = 0; i < MAX_AXIS; ++i) {
		axis[i].x = 0.0f;
		axis[i].y = 0.0f;
	}
	for (int i = 0; i < MAX_BUTTONS; ++i) {
		buttons[i] = 0.0f;
	}
}

AXIS	InputBase::GetAxis(unsigned int i) {
	if (i > MAX_AXIS) {
		return AXIS();
	}
	if (fabs(axis[i].x) < 0.05)
		axis[i].x = 0.0;
	if (fabs(axis[i].y) < 0.05)
		axis[i].y = 0.0;
	return axis[i];
}

float	InputBase::GetButton(unsigned int i) {
	if (i > MAX_BUTTONS) {
		return 0.0f;
	}
	return buttons[i];
}

bool	InputBase::GetButtonDown(unsigned int i) {
	if (i > MAX_BUTTONS) {
		return false;
	}
	return buttons[i] > 0.5f ? true : false;
}

bool	InputBase::GetButtonTriggered(unsigned int i) {
	if (i > MAX_BUTTONS) {
		return false;
	}
	return (GetButtonDown(i) && !GetButtonHeld(i));
}

bool	InputBase::GetButtonHeld(unsigned int i) {
	if (i > MAX_BUTTONS) {
		return false;
	}
	return buttonsHeld[i] > 0.5f ? true : false;
}

void InputBase::UpdateHolds()	{
	//memcpy(buttonsHeld, buttons, MAX_BUTTONS * sizeof(float));
	for (int i = 0; i < MAX_BUTTONS; i++)
	{
		buttonsHeld[i] = buttons[i];
	}
}