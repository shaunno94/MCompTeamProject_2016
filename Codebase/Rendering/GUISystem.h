#pragma once
#include "OrthoComponent.h"

class GUISystem
{
public:
	static bool Initialise();
	static void Destroy();
	static GUISystem& GetInstance(){ return *guiSystem; };

	void AddOrthoComponent(OrthoComponent* component);
	void RemoveOrthoComponent(OrthoComponent* component);
	//void Update();
	void Render();

	bool HasInitialised();

protected:
	static GUISystem* guiSystem;
	bool init;

private:
	GUISystem();
	~GUISystem(void);
	std::vector<OrthoComponent*> m_component;

};