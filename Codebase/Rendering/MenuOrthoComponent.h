#pragma once
#include "OrthoComponent.h"
#include "ButtonGUIComponent.h"

enum SELECTED
{
	SINGLE_PLAYER,
	MULTIPLAYER,
	QUIT
};

class MenuOrthoComponent: public OrthoComponent
{
public:
	MenuOrthoComponent(float z);
	virtual ~MenuOrthoComponent();

	virtual void Render() override;

	void SetSelection(unsigned int selection)
	{
		m_Selection = selection;
	};

	unsigned int GetSelection()
	{
		return m_Selection;
	}

	int Update();

protected:

	unsigned int m_Selection = SINGLE_PLAYER;
};