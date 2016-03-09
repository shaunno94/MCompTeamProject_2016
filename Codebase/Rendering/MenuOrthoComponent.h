#pragma once
#include "OrthoComponent.h"
#include "ButtonGUI.h"

enum SELECTED
{
	SINGLE_PLAYER,
	MULTIPLAYER,
	QUIT
};

class MenuOrthoComponent : public OrthoComponent
{
public:
	MenuOrthoComponent(ButtonGUI* single, ButtonGUI* multi, ButtonGUI* exit, float z);
	virtual ~MenuOrthoComponent();

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

	ButtonGUI* m_SingleBtn;
	ButtonGUI* m_MultiBtn;
	ButtonGUI* m_ExitBtn;

	unsigned int m_Selection = SINGLE_PLAYER;
};