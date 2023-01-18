#pragma once

#include <irrKlang.h>

class SoundManager
{
public:
	SoundManager();
	~SoundManager();

public:
	static void Init();
	static void Play2D(const std::string& key, bool isLoop);
	static void PlayBGM(int index);
	static void AddSound(const std::string& key, const std::string& path);
	static std::string& GetSound(const std::string& key);
	static bool IsPlaying(const std::string& key);

private:
	static irrklang::ISoundEngine* soundEngine;

	static std::map<std::string, std::string> sounds;
};