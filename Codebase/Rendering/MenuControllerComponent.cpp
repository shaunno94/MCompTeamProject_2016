#include "MenuControllerComponent.h"

MenuControllerComponent::MenuControllerComponent(MenuGUI* parent, unsigned int type)
{
	m_parent = parent;
}

MenuControllerComponent::~MenuControllerComponent()
{

}

void MenuControllerComponent::UpdateObject(float dt)
{
	
}

void MenuControllerComponent::SelectNext()
{
	int selection = (m_parent->GetSelection() + 1) % 3;
	m_parent->SetSelection(selection);
}

void MenuControllerComponent::SelectPrevious()
{
	int selection = m_parent->GetSelection() - 1;
	if (selection < 0)
		selection = 2;
	m_parent->SetSelection(selection);
}

void MenuControllerComponent::Submit()
{
	//TODO: Submit the selection
}