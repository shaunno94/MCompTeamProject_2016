#pragma once
#include "GUIComponent.h"
#include "GameObject.h"

enum SELECTED
{
	SINGLE_PLAYER,
	MULTIPLAYER,
	QUIT
};

class MenuGUI : public GUIComponent
{
public:
	MenuGUI(Material* material, Texture* texture, float z, const std::string& name="" ,unsigned int selected = SINGLE_PLAYER, bool visible = true);
	virtual ~MenuGUI();

	virtual void Render();

	void SetSelection(unsigned int selection)
	{
		m_Selection = selection;
	};

	unsigned int GetSelection()
	{
		return m_Selection;
	}

protected:
	GameObject* m_SingleBtn;
	GameObject* m_MultiBtn;
	GameObject* m_ExitBtn;

	Mesh* m_SingleMesh;
	Mesh* m_MultiMesh;
	Mesh* m_ExitMesh;

	unsigned int m_Selection;

};