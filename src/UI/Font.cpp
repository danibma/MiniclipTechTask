#include "Font.h"

Font::Font(const char* path, int32_t size) : m_Size(size)
{
	m_Font = TTF_OpenFont(path, size);
	if (!m_Font)
	{
		printf("Failed to open font: %s\n", TTF_GetError());
	}
}

void Font::CloseFont()
{
	TTF_CloseFont(m_Font);
}