#include "MenuControllerComponent.h"
#include "Renderer.h"
#include "GUISystem.h"
#include "Application\GameScene.h"

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

void MenuControllerComponent::Submit(std::vector<Scene*>& scene)
{

	switch (m_parent->GetSelection())
	{
	case SINGLE_PLAYER:
		Renderer::GetInstance()->SetCurrentScene(scene[1]);
		dynamic_cast<GameScene*>(Renderer::GetInstance()->GetCurrentScene())->Setup();
		break;
	case MULTIPLAYER:
		break;
	case QUIT:
		break;
	default:
		std::cout << "Selection Invalid!";
	}
}