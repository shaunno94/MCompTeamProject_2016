#pragma once
#include "GUIComponent.h"

class ButtonGUIComponent : public GUIComponent
{
public:
	ButtonGUIComponent();
	ButtonGUIComponent(Material* material, Material* matSelected, Vec3Graphics pos, Vec2Graphics scale, bool selected = false, bool visible = true);
	virtual ~ButtonGUIComponent();

	void Update();

	inline void SetSelected(bool selected) { m_Selected = selected; }
	inline bool GetSelected(){ return m_Selected; }

protected:
	Material* m_MatSelected;

	Mesh* m_Mesh;

	Texture* m_Texture;
	Texture* m_TexSelected;

	bool m_Selected;
};