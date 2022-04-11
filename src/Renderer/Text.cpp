#include "Text.h"

Text::Text(const std::string& text, Font& font, SDL_Color color /*= WHITE_COLOR*/)
	: m_Text(text), m_Color(color), m_Font(font)
{
}

Text::~Text()
{
	m_Font.Destroy();
	SDL_DestroyTexture(m_Texture);
}
