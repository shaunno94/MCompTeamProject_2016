#include "SoundManager.h"
#include "Rendering/constants.h"

std::map<unsigned int, Sound*> SoundManager::soundAssets;

void SoundManager::AddSound(std::string fileName, unsigned int id)
{
	if (!GetSound(id))
	{
		Sound* s = new Sound();

		std::string extension = fileName.substr(fileName.length() - 3, 3);

		if (extension == "wav")
		{
			s->LoadFromWAV(fileName);
			alGenBuffers(1, &s->buffer);
			alBufferData(s->buffer, s->GetOALFormat(), s->GetData(), s->GetSize(), (ALsizei)s->GetFrequency());
		}
		else if (extension == "ogg") {
			OggSound* ogg = new OggSound();
			ogg->LoadFromOgg(fileName);

			s = ogg;
		}
		else
		{
			std::cout << "Invalid extension " << extension << "!" << std::endl;
		}
		soundAssets.insert(std::make_pair(id, s));
	}
}

Sound* SoundManager::GetSound(unsigned int id)
{
	std::map<unsigned int, Sound*>::iterator s = soundAssets.find(id);
	return (s != soundAssets.end() ? s->second : NULL);
}

void SoundManager::DeleteSounds()
{
	for (std::map<unsigned int, Sound*>::iterator i = soundAssets.begin(); i != soundAssets.end(); ++i)
	{
		delete i->second;
	}
}

void SoundManager::LoadAssets()
{
	AddSound(AUDIO_DIR"misc/bang.wav", GUN);
	AddSound(AUDIO_DIR"misc/bang.wav", WALK);
	AddSound(AUDIO_DIR"music/votives.wav", SONG);
	AddSound(AUDIO_DIR"car/engine_idle.wav", ENGINE_IDLE);
	AddSound(AUDIO_DIR"car/engine_rev.wav", ENGINE_REV);
}