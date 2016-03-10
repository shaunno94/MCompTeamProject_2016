#pragma once
#include "GUIComponent.h"

class  TextGUIComponent : public GUIComponent
{
public:
	TextGUIComponent();
	TextGUIComponent(Material* material, const std::string& text, Vec3Graphics pos, Vec3Graphics scale, bool visible = true);
	~TextGUIComponent();

	void Update(const std::string& text);


protected:
	Font* m_Font;
	Mesh* m_Mesh;
	Texture* m_Texture;
	RenderComponent* m_RenderComp;

};