#include "ParticleManager.h"

ParticleManager* ParticleManager::particleManager = nullptr;
bool ParticleManager::init = false;

ParticleManager* ParticleManager::Instance()
{
	if (!particleManager) 
	{
		particleManager = new ParticleManager();
		init = true;
	}
	return particleManager;
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
		m_Systems[i]->Update(delta);
	}
}

void ParticleManager::Destroy()
{
	if (particleManager)
	{
		for (auto& system : particleManager->m_Systems)
		{
			delete system;
			system = nullptr;
		}
		particleManager->m_Systems.clear();
		delete particleManager;
		particleManager = nullptr;
		init = false;
	}
}