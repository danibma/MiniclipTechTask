#pragma once

#include <string>

#include <SDL_ttf.h>

class Font
{
public:
	Font(const char* path, int32_t size);
	~Font();

	inline TTF_Font* GetTTFFont() const { return m_Font; }
	inline const int32_t GetSize() const { return m_Size; }

	void Destroy();

private:
	TTF_Font* m_Font = nullptr;
	int32_t	m_Size = 0;
};

