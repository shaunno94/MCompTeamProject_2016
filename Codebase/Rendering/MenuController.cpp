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
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_UP) || Window::GetKeyboard()->KeyTriggered(KEYBOARD_W))
	{
		std::cout << "UP" << std::endl;
	}
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_DOWN) || Window::GetKeyboard()->KeyTriggered(KEYBOARD_S))
	{
		std::cout << "DOWN" << std::endl;
	}

	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_RETURN))
	{
		std::cout << "RETURN" << std::endl;
	}
}

#endif