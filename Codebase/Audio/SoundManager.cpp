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
#ifndef ORBIS
			alGenBuffers(1, &s->buffer);
			alBufferData(s->buffer, s->GetOALFormat(), s->GetData(), s->GetSize(), (ALsizei)s->GetFrequency());
		}
		else if (extension == "ogg") {
			OggSound* ogg = new OggSound();
			ogg->LoadFromOgg(fileName);

			s = ogg;
#endif
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
	AddSound(AUDIO_DIR"misc/bang_norm.wav", BANG);
	AddSound(AUDIO_DIR"music/Red_Doors_2_Norm.wav", SONG);
	AddSound(AUDIO_DIR"music/votives.ogg", SONG2);
	AddSound(AUDIO_DIR"car/engine_idle_norm.wav", ENGINE_IDLE);
	AddSound(AUDIO_DIR"car/engine_rev_norm.wav", ENGINE_REV);
	AddSound(AUDIO_DIR"car/launch_norm.wav", JUMP);
	AddSound(AUDIO_DIR"car/alien_sucker2_norm.wav", POWERUP);
	AddSound(AUDIO_DIR"car/boost_norm.wav", BOOST);


}
