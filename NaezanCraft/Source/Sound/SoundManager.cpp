#include "../pch.h"
#include "SoundManager.h"

irrklang::ISoundEngine* SoundManager::soundEngine;
std::map<std::string, std::string> SoundManager::sounds;

SoundManager::SoundManager() = default;

SoundManager::~SoundManager() = default;

void SoundManager::Init()
{
	soundEngine = irrklang::createIrrKlangDevice();
}

void SoundManager::Play2D(const std::string& key, bool isLoop)
{
	soundEngine->play2D(sounds[key].c_str(), isLoop);
}

void SoundManager::PlayBGM(int index)
{
	std::string s = "BGM" + std::to_string(index);
	Play2D(s, false);
}

void SoundManager::AddSound(const std::string& key, const std::string& path)
{
	sounds.emplace(key, path);
}

std::string& SoundManager::GetSound(const std::string& key)
{
	return sounds[key];
}

bool SoundManager::IsPlaying(const std::string& key)
{
	return soundEngine->isCurrentlyPlaying(sounds[key].c_str());
}
