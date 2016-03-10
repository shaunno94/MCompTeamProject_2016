#pragma once
#include "ParticleSystem.h"

class ParticleManager
{
public:
	static ParticleManager* Instance();
	static void Destroy();

	void AddSystem(ParticleSystem* system);
	void RemoveSystem(ParticleSystem* system);
	void Update(float delta);
	bool HasInitialised();

protected:
	static ParticleManager* particleManager;
	static bool init;

private:
	ParticleManager() {}
	~ParticleManager(void) {}
	std::vector<ParticleSystem*> m_Systems;
};