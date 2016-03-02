#pragma once
#include "Audio/SoundSystem.h"

class GameObject;

class AudioComponent {
public:
	AudioComponent(){}
	virtual ~AudioComponent(){}

	virtual void Update() = 0;

	void SetObject(GameObject* obj);

protected:

	void AddEmitters();

	// Bulk functions
	void SetPositions(const Vec3& pos);
	void SetVelocities(const Vec3& vel);
	void SetRadius(float rad);

	std::vector<SoundEmitter*> m_AudioList;
	GameObject* m_Object;
};

class AudioCompCar : public AudioComponent {
public:
	AudioCompCar(bool global);
	virtual ~AudioCompCar(){}

	virtual void Update() override;
};