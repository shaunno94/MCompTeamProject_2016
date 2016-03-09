#include "MenuOrthoComponent.h"

MenuOrthoComponent::MenuOrthoComponent(float z) : OrthoComponent(z)
{

}

MenuOrthoComponent::~MenuOrthoComponent()
{

}

int MenuOrthoComponent::Update()
{
	for (unsigned int i = 0; i < m_elements.size(); i++)
	{
		dynamic_cast<ButtonGUIComponent*> (m_elements[i])->SetSelected(false);

	}

	switch (m_Selection)
	{
	case SINGLE_PLAYER:
		dynamic_cast<ButtonGUIComponent*>(m_elements[0])->SetSelected(true);
		break;
	case MULTIPLAYER:
		dynamic_cast<ButtonGUIComponent*>(m_elements[1])->SetSelected(true);
		break;
	case QUIT:
		dynamic_cast<ButtonGUIComponent*>(m_elements[2])->SetSelected(true);
		break;
	default:
		std::cout << "Option invalid!" << std::endl;
	}

	for (unsigned int i = 0; i < m_elements.size(); i++)
	{
		dynamic_cast<ButtonGUIComponent*>(m_elements[i])->Update();
	}

	return m_Selection;
}

void MenuOrthoComponent::Render()
{
	for (unsigned int i = 0; i < m_elements.size(); i++)
	{
		dynamic_cast<ButtonGUIComponent*> (m_elements[i])->Render();
	}
}