#include "GUIComponent.h"

class ButtonGUI : public GUIComponent
{
	ButtonGUI(float depth);
	virtual ~ButtonGUI();

	void Update();
	void Render();

};
