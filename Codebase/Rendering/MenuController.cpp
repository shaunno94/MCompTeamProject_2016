#ifndef ORBIS
#include "MenuController.h"


MenuController::MenuController(ControllerComponent* object)
{
	setObject(object);
}


MenuController::~MenuController()
{

}

void MenuController::CheckInput()
{
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_UP) || Window::GetKeyboard()->KeyDown(KEYBOARD_W))
	{
	//TODO: Check if top reached, if not, move menu selection up
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_DOWN) || Window::GetKeyboard()->KeyDown(KEYBOARD_S))
	{
	//TODO: Check if bottom reached, if not, move menu selection down
	}

	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_RETURN))
	{
	//TODO: Execute the current selected button
	}
}

#endif