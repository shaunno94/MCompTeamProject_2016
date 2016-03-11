#include "NetServerControllerComponent.h"
#include "Renderer.h"
#include "GUISystem.h"
#include "Application\GameScene.h"
#include "Application\constants.h"

NetServerControllerComponent::NetServerControllerComponent(MenuOrthoComponent* parent, unsigned int type)
{
	m_parent = parent;
}

NetServerControllerComponent ::~NetServerControllerComponent()
{

}

void NetServerControllerComponent::UpdateObject(float dt)
{

}

void NetServerControllerComponent::SelectNext()
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

void NetServerControllerComponent::SelectPrevious()
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

void NetServerControllerComponent::Submit()
{

	switch (m_parent->GetSelection())
	{
	case SERVER_MENU_BACK:
		Renderer::GetInstance()->SetCurrentScene(Renderer::GetInstance()->GetScene(MENU_SCENE));
		break;
	case SERVER_MENU_START:
		Renderer::GetInstance()->SetCurrentScene(Renderer::GetInstance()->GetScene(SERVER_GAME_SCENE));
		break;
	default:
		std::cout << "Selection Invalid!";
	}
}