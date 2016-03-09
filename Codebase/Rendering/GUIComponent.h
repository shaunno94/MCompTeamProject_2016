#pragma once
#include "GameObject.h"

class GUIComponent : public GameObject
{
public:
	GUIComponent(Material* material, bool visible, const std::string& name = "") : GameObject(name) 
	{
		m_Material = material;
		m_Visible = visible;
	};

	virtual ~GUIComponent()
	{
		delete m_Material;
	}

	//virtual void Update() = 0;
	virtual void Render()
	{
		GetRenderComponent()->Draw();
	};


protected:
	bool	m_Visible;

	Material*	m_Material;

};