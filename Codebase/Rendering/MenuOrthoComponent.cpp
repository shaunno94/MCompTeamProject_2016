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
		auto button = dynamic_cast<ButtonGUIComponent*> (m_elements[i]);
		if (button)
		{
			button->SetSelected(false);
		}

	}

	dynamic_cast<ButtonGUIComponent*>(m_elements[m_Selection])->SetSelected(true);

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
		for (int j = 0; j < m_elements[i]->GetChildren().size(); j++)
		{
			dynamic_cast<GUIComponent*>(m_elements[i]->GetChildren()[j])->Render();
		}
		m_elements[i]->Render();
	}
}