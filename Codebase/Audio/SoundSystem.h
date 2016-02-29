#pragma once

#include "SoundEmitter.h"

class SoundSystem
{
public:
	// singleton methods
	static void Initialise(unsigned int channels = 32)
	{
		instance = new SoundSystem(channels);
	}
	static void Release()
	{
		delete instance;
	}
	static SoundSystem* Instance()
	{
		return instance;
	}

	//
	void AddSoundEmitter(SoundEmitter* s)
	{
		totalEmitters.push_back(s);
	}

	void SetListenerMatrix(const Mat4& transform);
	void SetListenerVelocity(const Vec3& vel) {
		listenerVel = vel;
	}

	Mat4 GetListenerTransform() { return listenerTransform; }

	void		Update(float msec);

	void		SetMasterVolume(float value);

	void		Play(Sound* s, SoundMOD modifier);

protected:
	SoundSystem(unsigned int channels = 32);
	~SoundSystem(void);

	void		UpdateListener();

	void AttachSources(int numSources);

	void		CullNodes();

	OALSource*	GetSource();

	std::vector<OALSource*>	sources;

	std::vector<SoundEmitter*>	emitters;
	std::vector<SoundEmitter*>	totalEmitters;

	Mat4 listenerTransform = Mat4(); // world transform
	Vec3 listenerPos = Vec3(); // position
	Vec3 listenerVel = Vec3(); // velocity

	ALCcontext*			context;
	ALCdevice*			device;

	float				masterVolume;

	static SoundSystem* instance;
};