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

void GUISystem::AddOrthoElement(GUIOrthoElements* component)
{
	m_elements.push_back(component);
}

void GUISystem::AddOrthoElement(GUIOrthoElements* component)
{
	for (unsigned int i = 0; i < m_elements.size(); ++i)
	{
		if (m_elements[i] == component)
			m_elements.erase(m_elements.begin());
	}
}

void GUISystem::Update(float delta)
{
	for (unsigned int i = 0; i < m_elements.size(); ++i)
		m_elements[i]->Update(delta);
}

void GUISystem::Render(float delta)
{
	for (unsigned int i = 0; i < m_elements.size(); ++i)
		m_elements[i]->Render(delta);
}

void GUISystem::Shutdown()
{
	m_elements.clear();
}
