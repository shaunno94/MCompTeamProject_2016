#pragma once

#include <vector>
#include <map>
#include <algorithm>

#include "Sound.h"

class SoundManager
{
public:
	static void AddSound(std::string name);
	static Sound* GetSound(std::string name);

	static void DeleteSounds();
protected:
	SoundManager() {}
	~SoundManager() {}

	static std::map<std::string, Sound*> soundAssets;
};