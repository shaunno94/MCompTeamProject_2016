#include "GUISystem.h"

GUISystem* GUISystem::guiSystem;

GUISystem::GUISystem()
{
	init = true;
}

bool GUISystem::Initialise()	{
	guiSystem = new GUISystem();

	if (!guiSystem->HasInitialised()) {
		return false;
	}
	return true;
}

bool GUISystem::HasInitialised()
{
	return init;
}

void GUISystem::AddOrthoComponent(OrthoComponent* component)
{
	m_component.push_back(component);
}

void GUISystem::RemoveOrthoComponent(OrthoComponent* component)
{
	for (unsigned int i = 0; i < m_component.size(); ++i)
	{
		if (m_component[i] == component)
			m_component.erase(m_component.begin());
	}
}

void GUISystem::Update()
{
	for (unsigned int i = 0; i < m_component.size(); ++i)
		m_component[i]->Update();
}

void GUISystem::Render()
{
	std::sort(m_component.begin(), m_component.end());

	for (unsigned int i = 0; i < m_component.size(); ++i)
		m_component[i]->Render();
}

void GUISystem::Destroy()
{
	guiSystem->m_component.clear();
}
