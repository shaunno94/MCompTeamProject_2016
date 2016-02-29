#pragma once

#include <vector>
#include <map>
#include <algorithm>

#include "Sound.h"
#include "OggSound.h"

class SoundManager
{
public:
	static void AddSound(std::string fileName, unsigned int id);
	static Sound* GetSound(unsigned int id);

	static void DeleteSounds();
protected:
	SoundManager() {}
	~SoundManager() {}

	static std::map<unsigned int, Sound*> soundAssets;
};