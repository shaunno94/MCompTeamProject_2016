#pragma once

#include "SoundEmitter.h"

class SoundSystem
{
public:
	static void Initialise(unsigned int channels = 32)
	{
		instance = new SoundSystem(channels);
	}
	static void Destroy()
	{
		delete instance;
	}
	static SoundSystem* Instance()
	{
		return instance;
	}
	void AddSoundEmitter(SoundEmitter* s)
	{
		totalEmitters.push_back(s);
	}

	void SetListener(const Mat4& transform);

	Mat4 GetListenerTransform() { return listenerTransform; }

	void		Update(float msec);

	void		SetMasterVolume(float value);

	void		Play(Sound* s, SoundMOD modifier);

protected:
	SoundSystem(unsigned int channels = 32);
	~SoundSystem(void);

	void		UpdateListener();

	void		DetachSources(std::vector<SoundEmitter*>::iterator from, std::vector<SoundEmitter*>::iterator to);
	void		AttachSources(std::vector<SoundEmitter*>::iterator from, std::vector<SoundEmitter*>::iterator to);

	void		CullNodes();

	OALSource*	GetSource();

	std::vector<OALSource*>	sources;

	std::vector<SoundEmitter*>	emitters;
	std::vector<SoundEmitter*>	totalEmitters;

	Mat4 listenerTransform; // world transform
	Vec3 listenerPos; // position

	ALCcontext*			context;
	ALCdevice*			device;

	float				masterVolume;

	static SoundSystem* instance;
};