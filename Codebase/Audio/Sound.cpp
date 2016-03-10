#include "Sound.h"
/* BasicSound */
BasicSound::BasicSound()
{
	bitRate = 0;
	freqRate = 0;
	length = 0;
	data = NULL;
}

BasicSound::~BasicSound(void)
{
	delete data;
}

void	BasicSound::LoadFromWAV(std::string filename)
{
	std::ifstream	file(filename.c_str(), std::ios::in | std::ios::binary);

	if (!file)
	{
		std::cout << "Failed to load WAV file '" << filename << "'!" << std::endl;
		return;
	}

	std::string		 chunkName;
	unsigned int chunkSize;

	while (!file.eof())
	{
		LoadWAVChunkInfo(file, chunkName, chunkSize);

		if (chunkName == "RIFF")
		{
			file.seekg(4, std::ios_base::cur);
			//char waveString[4];
			//file.read((char*)&waveString,4);
		}
		else if (chunkName == "fmt ")
		{
			FMTCHUNK fmt;

			file.read((char*)&fmt, sizeof(FMTCHUNK));

			bitRate = fmt.samp;
			freqRate = (float)fmt.srate;
			channels = fmt.channels;
		}
		else if (chunkName == "data")
		{
			size = chunkSize;
			data = new char[size];
			file.read((char*)data, chunkSize);
			break;
			/*
			In release mode, ifstream and / or something else were combining
			to make this function see another 'data' chunk, filled with
			nonsense data, breaking WAV loading. Easiest way to get around it
			was to simply break after loading the data chunk. This *should*
			be fine for any WAV file you find / use. Not fun to debug.
			*/
		}
		else
		{
			file.seekg(chunkSize, std::ios_base::cur);
		}
	}

	length = (float)size / (channels * freqRate * (bitRate / 8.0f)) * 1000.0f;

	file.close();
}

void	BasicSound::LoadWAVChunkInfo(std::ifstream& file, std::string& name, unsigned int& size)
{
	char chunk[4];
	file.read((char*)&chunk, 4);
	file.read((char*)&size, 4);

	name = std::string(chunk, 4);
}

/* Sound */
#ifndef ORBIS
Sound::Sound()
{
	buffer = 0;
	streaming = false;
}

Sound::~Sound()
{
	alDeleteBuffers(1, &buffer);
}

ALenum Sound::GetOALFormat()
{
	if (GetBitRate() == 16)
	{
		return GetChannels() == 2 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;
	}
	else if (GetBitRate() == 8)
	{
		return GetChannels() == 2 ? AL_FORMAT_STEREO8 : AL_FORMAT_MONO8;
	}
	return AL_FORMAT_MONO8;
}
#endif // !ORBIS
