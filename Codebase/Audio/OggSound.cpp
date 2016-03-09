#ifndef ORBIS
#include "OggSound.h"

OggSound::OggSound(void)	{
	streaming = true;
	fileHandle = NULL;
}

OggSound::~OggSound(void)	{
	if (fileHandle) {
		ov_clear(&streamHandle);
		fclose(fileHandle);
	}
}

void	OggSound::LoadFromOgg(std::string filename) {
	fileHandle = fopen(filename.c_str(), "rb");

	if (!fileHandle) {
		std::cout << "Failed to load OGG file '" << filename << "'!" << std::endl;
		return;
	}

	if (ov_open(fileHandle, &streamHandle, NULL, 0) < 0) {
		std::cout << "Failed to get OGG stream handle!" << std::endl;
		return;
	}

	vorbis_info* vorbisInfo = ov_info(&streamHandle, -1);

	channels = vorbisInfo->channels;
	freqRate = (float)vorbisInfo->rate;
	//bitRate		= (unsigned int)vorbisInfo->bitrate_nominal / 10000;
	bitRate = 16;	//For now!

	length = (float)ov_time_total(&streamHandle, -1) * 1000.0f;

	ov_time_seek(&streamHandle, 0);
}

double	OggSound::StreamData(ALuint	buffer, double timeLeft) {
	char	data[BUFFERLENGTH];
	int		read = 0;
	int		readResult = 0;
	int		section;

	int seek = ov_time_seek(&streamHandle, (length - timeLeft) / 1000.0);

	if (seek != 0) {
		return StreamData(buffer, timeLeft + GetLength());
	}

	while (read < BUFFERLENGTH) {
		readResult = ov_read(&streamHandle, data + read, BUFFERLENGTH - read, 0, 2, 1, &section);

		if (readResult > 0) {
			read += readResult;
		}
		else {
			break;
		}
	}

	if (read > 0) {
		alBufferData(buffer, GetOALFormat(), data, read, GetFrequency());
	}

	return (float)read / (channels * freqRate * (bitRate / 8.0)) * 1000.0f;
}
#endif