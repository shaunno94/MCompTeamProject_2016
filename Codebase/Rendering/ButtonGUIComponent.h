#pragma once
#include "GUIComponent.h"
#include "TextGUIComponent.h"

class ButtonGUIComponent : public GUIComponent
{
public:
	ButtonGUIComponent();
	ButtonGUIComponent(Material* material, Material* matSelected, Vec3Graphics pos, Vec2Graphics scale, bool selected = false, bool visible = true);
	virtual ~ButtonGUIComponent();

	void Update();

	inline void SetSelected(bool selected) { m_Selected = selected; }
	inline bool GetSelected(){ return m_Selected; }

	void AddText(Material* textMaterial, const std::string& text);

protected:
	Material* m_MatSelected;
	
	Mesh* m_Mesh;

	Texture* m_Texture;
	Texture* m_TexSelected;

	TextGUIComponent* m_Text;

	bool m_Selected;
	Vec3Graphics m_pos;
	Vec2Graphics m_scale;
};