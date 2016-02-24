#ifndef ORBIS
#pragma once
#include "ParticleSystem.h"

class ParticleManager
{
public:
	static bool Initialise();
	static void Destroy();
	static ParticleManager& GetManager(){ return *particleManager; };

	void AddSystem(ParticleSystem* system);
	void RemoveSystem(ParticleSystem* system);
	void Render(float delta);
	void Shutdown();

	bool HasInitialised();

protected:
	void Update(float delta);

	static ParticleManager* particleManager;
	bool init;
private:
	ParticleManager();
	~ParticleManager(void);
	std::vector<ParticleSystem*> m_Systems;
};
#endif