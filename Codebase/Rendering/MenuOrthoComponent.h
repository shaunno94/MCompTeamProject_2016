#pragma once
#include "OrthoComponent.h"
#include "ButtonGUIComponent.h"



class MenuOrthoComponent: public OrthoComponent
{
public:
	MenuOrthoComponent(float z);
	virtual ~MenuOrthoComponent();

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

	unsigned int m_Selection = 0;
};