#include "GUIComponent.h"

class MenuGUI : public GUIComponent
{
public:
	MenuGUI(Material* material, Texture* texture, float z, bool visible = true);
	virtual ~MenuGUI();

	virtual void Render();

protected:
	GameObject* background;
	Mesh* bg;
	RenderComponent* text_renderComp;
};