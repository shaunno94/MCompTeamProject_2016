#include "GUIComponent.h"

class HUD_GUI : public GUIComponent
{
public:
	HUD_GUI();
	~HUD_GUI();

	bool Update(float dt);
	void Render();

};