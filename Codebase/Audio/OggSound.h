#ifndef ORBIS
#pragma once
#include "sound.h"

#include "libvorbis-1.3.2/include/ogg/ogg.h"
#include "libvorbis-1.3.2/include/vorbis/codec.h"
#include "libvorbis-1.3.2/include/vorbis/vorbisenc.h"
#include "libvorbis-1.3.2/include/vorbis/vorbisfile.h"

#pragma comment(lib, "libvorbis-1.3.2/libogg_static.lib")
#pragma comment(lib, "libvorbis-1.3.2/libvorbis_static.lib")
#pragma comment(lib, "libvorbis-1.3.2/libvorbisfile_static.lib")

#define BUFFERLENGTH 32768	//Part 2

class OggSound : public Sound	{
public:
	OggSound(void);
	virtual ~OggSound(void);

	void			LoadFromOgg(std::string filename);
	virtual double	StreamData(ALuint	buffer, double timeLeft);

protected:
	FILE*			fileHandle;
	OggVorbis_File	streamHandle;
};
#endif
