#ifndef ORBIS
#pragma once

#include "SoundManager.h"

#define NUM_STREAM_BUFFERS 3

// default sound modifiers
#define SOUND_VOLUME 1.0f
#define SOUND_RADIUS 500.0f
#define SOUND_PITCH 1.0f
#define SOUND_LOOP true
#define SOUND_GLOBAL false

enum SoundPriority
{
	SOUNDPRIORTY_LOW,
	SOUNDPRIORITY_MEDIUM,
	SOUNDPRIORITY_HIGH,
	SOUNDPRIORITY_ALWAYS
};

struct OALSource
{
	ALuint	source;
	bool	inUse;

	OALSource(ALuint src)
	{
		source = src;
		inUse = false;
	}
};

struct SoundMOD {
	float volume = SOUND_VOLUME;
	float radius = SOUND_RADIUS;
	float pitch = SOUND_PITCH;
	bool looping = SOUND_LOOP;
	bool isGlobal = SOUND_GLOBAL;
	SoundPriority priority = SOUNDPRIORITY_HIGH;

	Vec3 position = Vec3();
};

class SoundEmitter
{
public:
	SoundEmitter(void);
	SoundEmitter(Sound* s);
	~SoundEmitter(void);

	void Reset();

	void SetSound(Sound* s);
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

	void SetIsGlobal(bool value) { isGlobal = value; }
	bool GetIsGlobal() const { return isGlobal; }
	
	void SetPosition(const Vec3& pos) {
		position = pos;
	}
	Vec3 GetPosition() const 
	{
		return position;
	}

	void setIsSingle(bool state) { isSingle = state; }
	bool GetIsSingle() const { return isSingle; }
	
	void SetVelocity(const Vec3& vel) {
		velocity = vel;
	}

	double GetTimeLeft()
	{
		return timeLeft;
	}

	OALSource* GetSource()
	{
		return oalSource;
	}

	static bool		CompareNodesByPriority(SoundEmitter* a, SoundEmitter* b);

	void			AttachSource(OALSource* s);
	void			DetachSource();

	virtual void	Update(float msec);

protected:
	Sound*			sound;
	OALSource*		oalSource;
	SoundPriority	priority;
	float			volume;
	float			radius;
	float			timeLeft;
	float pitch;

	bool isSingle;
	bool			isLooping;
	bool			isGlobal;

	double streamPos;
	ALuint streamBuffers[NUM_STREAM_BUFFERS];

	Vec3 position;
	Vec3 velocity;
};
#endif