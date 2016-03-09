#pragma once

#include <string>
#include <iostream>
#include <fstream>

#include "Math/nclglMath.h"

#ifndef ORBIS
#include "OpenAL 1.1 SDK/include/al.h"
#include "OpenAL 1.1 SDK/include/alc.h"

#pragma comment(lib, "OpenAL 1.1 SDK/libs/Win32/OpenAL32.lib")

typedef unsigned long DWORD;
#else
#include <audio3d.h>

typedef unsigned int DWORD;
#endif

struct FMTCHUNK
{
	short format;
	short channels;
	DWORD srate;
	DWORD bps;
	short balign;
	short samp;
};

class BasicSound
{

public:
	char*			GetData()
	{
		return data;
	}
	int				GetBitRate()
	{
		return bitRate;
	}
	float			GetFrequency()
	{
		return freqRate;
	}
	int				GetChannels()
	{
		return channels;
	}
	int				GetSize()
	{
		return size;
	}
	double			GetLength()
	{
		return length;
	}

protected:
	BasicSound();
	virtual ~BasicSound(void);

	void			LoadFromWAV(std::string filename);
	void			LoadWAVChunkInfo(std::ifstream& file, std::string& name, unsigned int& size);

	char*			data;

	float			freqRate;
	double			length;
	unsigned int	bitRate;
	unsigned int	size;
	unsigned int	channels;
};

// win32 one
#ifndef ORBIS
class Sound : public BasicSound
{
	friend class SoundManager;

public:
	ALuint			GetBuffer()
	{
		return buffer;
	}
	virtual double StreamData(unsigned int buffer, double timeleft)
	{
		return 0.0f;
	}
	bool IsStreaming()
	{
		return streaming;
	}
	ALenum			GetOALFormat();
protected:
	Sound();
	virtual ~Sound(void);

	ALuint			buffer;
	bool streaming;
};
#else
class Sound : public BasicSound
{
	friend class SoundManager;
};
#endif // !ORBIS

