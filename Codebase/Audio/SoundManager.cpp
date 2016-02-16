#include "SoundManager.h"

std::map<std::string, Sound*> SoundManager::soundAssets;

void SoundManager::AddSound(std::string name)
{
	if (!GetSound(name))
	{
		Sound* s = new Sound();

		std::string extension = name.substr(name.length() - 3, 3);

		if (extension == "wav")
		{
			s->LoadFromWAV(name);
			alGenBuffers(1, &s->buffer);
			alBufferData(s->buffer, s->GetOALFormat(), s->GetData(), s->GetSize(), (ALsizei)s->GetFrequency());
		}
		else
		{
			std::cout << "Invalid extension " << extension << "!" << std::endl;
		}
		soundAssets.insert(std::make_pair(name, s));
	}
}

Sound* SoundManager::GetSound(std::string name)
{
	std::map<std::string, Sound*>::iterator s = soundAssets.find(name);
	return (s != soundAssets.end() ? s->second : NULL);
}

void SoundManager::DeleteSounds()
{
	for (std::map<std::string, Sound*>::iterator i = soundAssets.begin(); i != soundAssets.end(); ++i)
	{
		delete i->second;
	}
}
