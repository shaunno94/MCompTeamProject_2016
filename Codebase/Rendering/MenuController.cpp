#ifndef ORBIS
#include "MenuController.h"


MenuController::MenuController(MenuControllerComponent* object)
{
	setObject(object);
}


MenuController::~MenuController()
{

}

void MenuController::CheckInput(std::vector<Scene*>& scenes)
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
		object->Submit(scenes);
	//	std::cout << "RETURN" << std::endl;
	}
}

#endif