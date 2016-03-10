#ifndef ORBIS
#include "MenuController.h"


MenuController::MenuController(UIControllerComponent* object)
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
		object->SelectPrevious();
	//	std::cout << "UP" << std::endl;
	
	}
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_DOWN) || Window::GetKeyboard()->KeyTriggered(KEYBOARD_S))
	{
		object->SelectNext();
	//	std::cout << "DOWN" << std::endl;
	}

	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_RETURN))
	{
		object->Submit();
	//	std::cout << "RETURN" << std::endl;
	}

}

#endif