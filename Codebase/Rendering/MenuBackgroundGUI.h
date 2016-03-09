#pragma once
#include "GUIComponent.h"

class MenuBackgroundGUI : public GUIComponent
{
public:
	MenuBackgroundGUI(Material* material,  bool visible = true);
	virtual ~MenuBackgroundGUI();

	virtual void Render();
protected:
	Texture* m_Texture;
	Mesh* m_Mesh;
};