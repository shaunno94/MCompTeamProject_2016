#include "NetClientControllerComponent.h"
#include "Renderer.h"
#include "GUISystem.h"
#include "Application\GameScene.h"
#include "Application\constants.h"

NetClientControllerComponent::NetClientControllerComponent(MenuOrthoComponent* parent, unsigned int type)
{
	m_parent = parent;
}

NetClientControllerComponent ::~NetClientControllerComponent()
{

}

void NetClientControllerComponent::UpdateObject(float dt)
{

}

void NetClientControllerComponent::SelectNext()
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

void NetClientControllerComponent::SelectPrevious()
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

void NetClientControllerComponent::Submit()
{

	switch (m_parent->GetSelection())
	{
	case CLIENT_MENU_BACK:
		Renderer::GetInstance()->SetCurrentScene(Renderer::GetInstance()->GetScene(MENU_SCENE));
		break;
	case CLIENT_MENU_START:
		Renderer::GetInstance()->SetCurrentScene(Renderer::GetInstance()->GetScene(CLIENT_GAME_SCENE));
		break;
	default:
		std::cout << "Selection Invalid!";
	}
}