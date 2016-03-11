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
	void SetPositions();
	Vec3 GetVec3Velocity();
	void SetVelocities();
	void SetRadius(float rad);

	std::vector<SoundEmitter*> m_AudioList;
	GameObject* m_Object;
};

class AudioCompCar : public AudioComponent {
public:
	AudioCompCar(bool global);
	virtual ~AudioCompCar(){}

	void SetBoostVolume(float val) {
		m_BoostVol = val;
	}
	virtual void Update() override;

protected:
	float m_Pitch;
	float m_BoostVol;
};

class AudioCompCarLitener : public AudioCompCar {
public:
	AudioCompCarLitener(bool global) : AudioCompCar(global) {}
	virtual ~AudioCompCarLitener(){}

	virtual void Update() override;
};
