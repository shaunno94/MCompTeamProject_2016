#pragma once
#include "GUIComponent.h"

class ButtonGUI : public GUIComponent
{
public:
	ButtonGUI();
	ButtonGUI(Material* material, Material* matSelected, Texture* texture, Texture* texSelected,
		float z, bool selected, Vec4Graphics* pos, Vec2Graphics* scale, bool visible = true);
	virtual ~ButtonGUI();

	virtual void Render();
	void Update(bool select);

	inline void SetSelected(bool selected) { m_Selected = selected; }
	inline bool GetSelected(){ return m_Selected; }

protected:
	Material* m_MatSelected;
	Texture* m_TexSelected;

	bool m_Selected;
};