#pragma once
#include "OrthoComponent.h"

class GUISystem
{
public:
	GUISystem();
	~GUISystem(void);

	void AddOrthoComponent(OrthoComponent* component);
	void RemoveOrthoComponent(OrthoComponent* component);
	void Render();

	bool HasInitialised();

protected:
	bool init;

private:
	std::vector<OrthoComponent*> m_component;

};