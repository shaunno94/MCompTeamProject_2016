#include "MenuControllerComponent.h"
#include "Renderer.h"
#include "GUISystem.h"
#include "Application\GameScene.h"
#include "Application\constants.h"


MenuControllerComponent::MenuControllerComponent(MenuOrthoComponent* parent, unsigned int type)
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
	unsigned int maxCount = m_parent->GetElements().size();
	unsigned int counter = 0;

	while (counter < maxCount)
	{
		counter++;
		unsigned int selection = (m_parent->GetSelection() + 1) % maxCount;
		auto selectable = dynamic_cast<ButtonGUIComponent*>(m_parent->GetElements()[selection]);
		if (selectable)
		{
			m_parent->SetSelection(selection);
			break;
		}
		else
			continue;
	}
}

void MenuControllerComponent::SelectPrevious()
{
	unsigned int maxCount = m_parent->GetElements().size();
	unsigned int counter = 0;

	while (counter < maxCount)
	{
		counter++;
		unsigned int selection = (m_parent->GetSelection() + maxCount - 1) % maxCount;
		auto selectable = dynamic_cast<ButtonGUIComponent*>(m_parent->GetElements()[selection]);
		if (selectable)
		{
			m_parent->SetSelection(selection);
			break;
		}
		else
			continue;
	}
	
}

void MenuControllerComponent::Submit()
{

	switch (m_parent->GetSelection())
	{
	case SINGLE_PLAYER:
		Renderer::GetInstance()->SetCurrentScene(Renderer::GetInstance()->GetScene(GAME_SCENE));
		break;
	case MULTIPLAYER:
		Renderer::GetInstance()->SetCurrentScene(Renderer::GetInstance()->GetScene(SERVER_SETUP_SCENE));
		break;
	case QUIT:
		break;
	default:
		std::cout << "Selection Invalid!";
	}
}