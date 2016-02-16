#pragma once

#include <string>
#include <iostream>
#include <fstream>

#include "Math/nclglMath.h"

#include "OpenAL 1.1 SDK/include/al.h"
#include "OpenAL 1.1 SDK/include/alc.h"

#pragma comment(lib, "OpenAL 1.1 SDK/libs/Win32/OpenAL32.lib")

typedef unsigned long DWORD;

struct FMTCHUNK
{
	short format;
	short channels;
	DWORD srate;
	DWORD bps;
	short balign;
	short samp;
};

class Sound
{

	friend class SoundManager;
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
	ALuint			GetBuffer()
	{
		return buffer;
	}

	ALenum			GetOALFormat();
	double			GetLength();

protected:
	Sound();
	virtual ~Sound(void);

	void			LoadFromWAV(std::string filename);
	void			LoadWAVChunkInfo(std::ifstream& file, std::string& name, unsigned int& size);

	char*			data;
	ALuint			buffer;

	float			freqRate;
	double			length;
	unsigned int	bitRate;
	unsigned int	size;
	unsigned int	channels;
};