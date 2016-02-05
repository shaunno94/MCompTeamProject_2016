#pragma once
#include "ParticleSystem.h"

class ParticleManager
{
public:
	ParticleManager() {};
	~ParticleManager() {};

	void AddSystem(ParticleSystem* system);
	void RemoveSystem(ParticleSystem* system);
	void Update(float delta);
	void Render();
	void Shutdown();

	bool DoesExist();

protected:
private:
	std::vector<ParticleSystem*> m_Systems;
};