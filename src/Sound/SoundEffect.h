#pragma once
#include <SDL_mixer.h>

class SoundEffect
{
public:
	SoundEffect() = default;
	SoundEffect(const char* path);

	void Destroy();

	void Play();
private:
	Mix_Chunk* m_Sound = nullptr;
};

