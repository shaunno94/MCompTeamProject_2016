#include "ParticleManager.h"

ParticleManager* ParticleManager::particleManager;

ParticleManager ::ParticleManager()
{
	init = true;
}

bool ParticleManager::Initialise()	{
	particleManager = new ParticleManager();

	if (!particleManager->HasInitialised()) {
		return false;
	}
	return true;
}

bool ParticleManager::HasInitialised()
{
	return init;
}

void ParticleManager::AddSystem(ParticleSystem* system)
{
	m_Systems.push_back(system);
}

void ParticleManager::RemoveSystem(ParticleSystem* system)
{
	for (unsigned int i = 0; i < m_Systems.size(); ++i)
	{
		if (m_Systems[i] == system)
			m_Systems.erase(m_Systems.begin());
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

void ParticleManager::Render(float delta)
{
	Update(delta);

	for (unsigned int i = 0; i < m_Systems.size(); ++i)
		m_Systems[i]->Render();
}

void ParticleManager::Shutdown()
{
	m_Systems.clear();
}