#pragma once

#include "SoundManager.h"

#define NUM_STREAM_BUFFERS 3

// default sound modifiers
#define SOUND_VOLUME 1.0f
#define SOUND_RADIUS 500.0f
#define SOUND_PITCH 1.0f
#define SOUND_LOOP true
#define SOUND_GLOBAL false

// ps4
#define SAMPLE_GRANULARITY 1024

enum SoundPriority
{
	SOUNDPRIORTY_LOW,
	SOUNDPRIORITY_MEDIUM,
	SOUNDPRIORITY_HIGH,
	SOUNDPRIORITY_ALWAYS
};

struct AudioSource
{
	unsigned int source;
	bool	inUse;

	AudioSource(unsigned int src)
	{
		source = src;
		inUse = false;
	}
};

struct SoundMOD
{
	float volume = SOUND_VOLUME;
	float radius = SOUND_RADIUS;
	float pitch = SOUND_PITCH;
	bool looping = SOUND_LOOP;
	bool isGlobal = SOUND_GLOBAL;
	SoundPriority priority = SOUNDPRIORITY_HIGH;
};

class Emitter
{
public:
	Emitter(void);
	~Emitter(void) {};

	virtual void Reset();
	virtual void SetSound(Sound* s) = 0;
	virtual void Update(float msec) = 0;
	virtual void AttachSource(AudioSource* s) = 0;
	virtual void DetachSource() = 0;

	static bool CompareNodesByPriority(Emitter* a, Emitter* b);

	Sound* GetSound()
	{
		return sound;
	}
	void SetPriority(SoundPriority p)
	{
		priority = p;
	}
	SoundPriority GetPriority() const
	{
		return priority;
	}
	void SetVolume(float value)
	{
		volume = value;
	}
	float GetVolume() const
	{
		return volume;
	}
	void SetLooping(bool state)
	{
		isLooping = state;
	}
	bool GetLooping() const
	{
		return isLooping;
	}
	void SetRadius(float value)
	{
		radius = value;
	}
	float GetRadius() const
	{
		return radius;
	}
	void SetPitch(float value)
	{
		pitch = value;
	}
	float GetPitch() const
	{
		return pitch;
	}
	void SetIsGlobal(bool value)
	{
		isGlobal = value;
	}
	bool GetIsGlobal() const
	{
		return isGlobal;
	}
	void setIsSingle(bool state)
	{
		isSingle = state;
	}
	bool GetIsSingle() const
	{
		return isSingle;
	}
	void SetPosition(const Vec3& pos)
	{
		position = pos;
	}
	Vec3 GetPosition() const
	{
		return position;
	}
	void SetVelocity(const Vec3& vel)
	{
		velocity = vel;
	}
	double GetTimeLeft() const
	{
		return timeLeft;
	}
	AudioSource* GetSource()
	{
		return currentSource;
	}

protected:
	Sound*			sound;
	AudioSource*	currentSource;

	SoundPriority	priority;
	float			volume;
	float			radius;
	float			pitch;
	float			timeLeft;

	bool			isSingle;
	bool			isLooping;
	bool			isGlobal;

	Vec3 position;
	Vec3 velocity;
};

#ifndef ORBIS
class SoundEmitter : public Emitter
{
public:
	SoundEmitter(void);
	SoundEmitter(Sound* s);
	~SoundEmitter(void);

	void Reset();
	void SetSound(Sound* s);
	void Update(float msec);
	void AttachSource(AudioSource* s);
	void DetachSource();

protected:

	double streamPos;
	ALuint streamBuffers[NUM_STREAM_BUFFERS];
};
#else

class SoundEmitter : public Emitter
{
public:
	SoundEmitter(void);
	SoundEmitter(Sound* s);
	~SoundEmitter(void) {}

	void Reset();
	void SetSound(Sound* s) { sound = s; }
	void Update(float msec);
	void AttachSource(AudioSource* s);
	void DetachSource();

	void SetPort(SceAudio3dPortId val)
	{
		port = val;
	}
	void SetSpread(float val) {
		spread = val;
	}

protected:
	void SampleFromSound(int16_t* output, int samplesPerChannel, int startSample);

	float spread;
	int samplesUsed;
	SceAudio3dPortId port;
};
#endif