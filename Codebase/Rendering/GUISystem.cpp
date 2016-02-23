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

void GUISystem::AddComponent(GUI_Component* component)
{
	m_Components.push_back(component);
}

void GUISystem::RemoveComponent(GUI_Component* component)
{
	for (unsigned int i = 0; i < m_Components.size(); ++i)
	{
		if (m_Components[i] == component)
			m_Components.erase(m_Components.begin());
	}
}

void GUISystem::Update(float delta)
{
	for (unsigned int i = 0; i < m_Components.size(); ++i)
	{
		if (!m_Components[i]->Update(delta))
		{
			delete m_Components[i];
		}
	}
}

void GUISystem::Render(float delta)
{
	Update(delta);

	for (unsigned int i = 0; i < m_Components.size(); ++i)
		m_Components[i]->Render();
}

void GUISystem::Shutdown()
{
	m_Components.clear();
}
