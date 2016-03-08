#include "MenuControllerComponent.h"

MenuControllerComponent::MenuControllerComponent(GameObject* parent, unsigned int type) :
	ControllerComponent(parent)
{
	parent->FindGameObject();
}

MenuControllerComponent::~MenuControllerComponent()
{

}

