#pragma once
#include "GUIComponent.h"
#include "GameObject.h"

class MenuBackgroundGUI : public GUIComponent
{
public:
	MenuBackgroundGUI(Material* material, Texture* texture, float z, bool visible = true);
	virtual ~MenuBackgroundGUI();

	virtual void Render();

protected:
	GameObject* background;
	Mesh* bg;
};