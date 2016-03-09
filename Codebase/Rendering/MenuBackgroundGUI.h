#pragma once
#include "GUIComponent.h"

class MenuBackgroundGUI : public GUIComponent
{
public:
	MenuBackgroundGUI(Material* material, Texture* texture, float z, bool visible = true);
	virtual ~MenuBackgroundGUI();

	virtual void Render();
};