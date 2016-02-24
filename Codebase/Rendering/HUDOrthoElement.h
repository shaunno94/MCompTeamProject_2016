#include "GUIOrthoElements.h"

class HUDOrthoElement : public GUIOrthoElements
{
public:
	HUDOrthoElement(float z);
	~HUDOrthoElement();

	bool Update(float dt);
	void Render(float dt);
};