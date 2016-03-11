#pragma once

#include "SoundEmitter.h"
#include <thread>
#include <atomic>

class SoundSystem
{
public:
	// singleton methods
	static void Initialise(unsigned int channels = 48)
	{
		if (!instance)
			instance = new SoundSystem(channels);
	}
	static void Release()
	{
		if (instance)
		{
			delete instance;
			instance = nullptr;
		}
	}
	static SoundSystem* Instance()
	{
		return instance;
	}

	void AddSoundEmitter(SoundEmitter* s)
	{
#ifdef ORBIS
		s->SetPort(audioPort);
#endif
		totalEmitters.push_back(s);
	}

	void SetListenerMatrix(const Mat4& transform);
	void SetListenerVelocity(const Vec3& vel)
	{
		listenerVel = vel;
	}

	Mat4 GetListenerTransform()
	{
		return listenerTransform;
	}

	void SetBackgroundMusic(Sound* snd);

	void SetBackgroundVolume(float val);

	void		Update(float msec);

	void		SetMasterVolume(float value);

	void		Play(Sound* s, SoundMOD modifier);

protected:
	SoundSystem(unsigned int channels = 64);
	~SoundSystem(void);

	void		UpdateListener();

	void AttachSources();

	void		CullNodes();

	AudioSource*	GetSource();

	std::vector<AudioSource*>	sources;

	SoundEmitter* m_Background;
	int m_MaxDynamicSources;

	std::vector<SoundEmitter*>	emitters;
	std::vector<SoundEmitter*>	totalEmitters;

	Mat4 listenerTransform = Mat4(); // world transform
	Vec3 listenerPos = Vec3(); // position
	Vec3 listenerVel = Vec3(); // velocity

#ifndef ORBIS
	ALCcontext*			context;
	ALCdevice*			device;
#else
	SceAudio3dPortId	audioPort;
#endif

	float				masterVolume;

	static SoundSystem* instance;
};