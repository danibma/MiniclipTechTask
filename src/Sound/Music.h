#pragma once
#include <SDL_mixer.h>

class Music
{
public:
	Music(const char* path);

	void Destroy();

	void Play(bool loop);
	void Pause();

private:
	Mix_Music* m_Sound;
};

