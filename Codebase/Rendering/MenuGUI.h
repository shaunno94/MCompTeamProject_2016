#pragma once
#include "GUIComponent.h"
#include "GameObject.h"

class MenuGUI : public GUIComponent
{
public:
	MenuGUI(Material* material, Texture* texture, float z, bool visible = true);
	virtual ~MenuGUI();

	virtual void Render();

protected:
	GameObject* m_SingleBtn;
	GameObject* m_MultiBtn;
	GameObject* m_ExitBtn;

	Mesh* m_SingleMesh;
	Mesh* m_MultiMesh;
	Mesh* m_ExitMesh;

};