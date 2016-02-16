#pragma once

#include "SoundEmitter.h"

class SoundSystem
{
public:
	static void Initialise(unsigned int channels = 8)
	{
		instance = new SoundSystem(channels);
	}

	static void Destroy()
	{
		delete instance;
	}

	static SoundSystem* GetSoundSystem()
	{
		return instance;
	}

	void AddSoundEmitter(SoundEmitter* s)
	{
		emitters.push_back(s);
	}
	
	void RemoveSoundEmitter(SoundEmitter* s);

	void ListenerUpdate(const Mat4& transform);

	Mat4 GetListenerTransform() { return listenerTransform; }

	void		Update(float msec);

	void		SetMasterVolume(float value);

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
	std::vector<SoundEmitter*>	frameEmitters;

	Mat4 listenerTransform; // world transform
	Vec3 listenerPos; // position

	ALCcontext*			context;
	ALCdevice*			device;

	float				masterVolume;

	static SoundSystem* instance;
};