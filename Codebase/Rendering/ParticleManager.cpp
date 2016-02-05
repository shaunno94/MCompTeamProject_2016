#include "ParticleManager.h"

void ParticleManager::AddSystem(ParticleSystem* system)
{
	m_Systems.push_back(system);
}

void ParticleManager::RemoveSystem(ParticleSystem* system)
{
	for (unsigned int i = 0; i < m_Systems.size(); ++i)
	{
		if (m_Systems[i] == system)
			m_Systems.erase[i];
	}
}

void ParticleManager::Update(float delta)
{
	for (unsigned int i = 0; i < m_Systems.size(); ++i)
	{
		if (!m_Systems[i]->Update(delta))
		{
			delete m_Systems[i];
		}
	}
}

void ParticleManager::Render()
{
	for (unsigned int i = 0; i < m_Systems.size(); ++i)
		m_Systems[i]->Render();
}

void ParticleManager::Shutdown()
{
	m_Systems.clear();
}

bool ParticleManager::DoesExist()
{
	if (m_Systems.empty)
		return false;
	else
		return true;
}