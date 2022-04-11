#include "SoundEffect.h"

#include <stdio.h>

SoundEffect::SoundEffect(const char* path)
{
	m_Sound = Mix_LoadWAV(path);
	if (!m_Sound)
		printf("Could not load sound: %s\n", SDL_GetError());
}

void SoundEffect::Destroy()
{
	Mix_FreeChunk(m_Sound);
}

void SoundEffect::Play()
{
	if (Mix_PlayChannel(-1, m_Sound, 0) == -1)
		printf("Could not play sound: %s\n", SDL_GetError());

	Mix_VolumeChunk(m_Sound, 20);
}