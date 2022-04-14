#include "Music.h"

#include <stdio.h>

Music::Music(const char* path)
{
	m_Sound = Mix_LoadMUS(path);
	if (!m_Sound)
		printf("Could not load sound: %s\n", SDL_GetError());
}

void Music::Destroy()
{
	Pause();
	Mix_FreeMusic(m_Sound);
}

void Music::Play(bool loop)
{
	if (!m_IsPlaying)
	{
		m_IsPlaying = true;

		if (Mix_PlayMusic(m_Sound, loop ? -1 : 0) == -1)
			printf("Could not play sound: %s\n", SDL_GetError());

		Mix_VolumeMusic(20);
	}
}

void Music::Pause()
{
	Mix_PauseMusic();
	m_IsPlaying = false;
}