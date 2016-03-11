#ifdef ORBIS
#include "PS4MenuController.h"


PS4MenuController::PS4MenuController(UIControllerComponent* object)
{
	setObject(object);
}


PS4MenuController::~PS4MenuController()
{

}

void PS4MenuController::CheckInput()
{
	if (PS4Input::getPS4Input()->GetButtonTriggered(BTN_CROSS))
		object->Submit();

	if (PS4Input::getPS4Input()->GetButtonTriggered(BTN_R1))
		object->SelectNext();

	if (PS4Input::getPS4Input()->GetButtonTriggered(BTN_L1))
		object->SelectPrevious();

}
#endif