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
	MenuGUI(Material* material, Texture* texture, float z, const std::string& name="" , bool visible = true);
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

	void Update();

protected:
	GameObject* m_SingleBtn;
	GameObject* m_MultiBtn;
	GameObject* m_ExitBtn;

	Mesh* m_SingleMesh;
	Mesh* m_MultiMesh;
	Mesh* m_ExitMesh;

	Texture* m_DefaultTex;
	Texture* m_SelectTex;

	unsigned int m_Selection = SINGLE_PLAYER;

};