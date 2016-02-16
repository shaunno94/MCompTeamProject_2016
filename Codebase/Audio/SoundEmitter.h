#pragma once

#include "SoundManager.h"

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

class SoundEmitter
{
public:
	SoundEmitter(void);
	SoundEmitter(Sound* s);
	~SoundEmitter(void);

	void			Reset();

	void			SetSound(Sound* s);
	Sound*			GetSound()
	{
		return sound;
	}

	void			SetPriority(SoundPriority p)
	{
		priority = p;
	}
	SoundPriority	GetPriority()
	{
		return priority;
	}

	void			SetVolume(float value)
	{
		volume = value;
	}
	float			GetVolume()
	{
		return volume;
	}

	void			SetLooping(bool state)
	{
		isLooping = state;
	}
	bool			GetLooping()
	{
		return isLooping;
	}

	void			SetRadius(float value)
	{
		radius = value;
	}
	float			GetRadius()
	{
		return radius;
	}

	double			GetTimeLeft()
	{
		return timeLeft;
	}

	Vec3 GetPosition()
	{
		return position;
	}

	void SetPitch(float value)
	{
		pitch = value;
	}

	float GetPitch()
	{
		return pitch;
	}

	OALSource*		GetSource()
	{
		return oalSource;
	}

	static bool		CompareNodesByPriority(SoundEmitter* a, SoundEmitter* b);

	void			AttachSource(OALSource* s);
	void			DetachSource();

	virtual void	Update(float msec);
	void SetPosition(const Vec3& pos);

protected:
	Sound*			sound;
	OALSource*		oalSource;
	SoundPriority	priority;
	float			volume;
	float			radius;
	float			timeLeft;
	bool			isLooping;

	float pitch;
	Vec3 position;
};