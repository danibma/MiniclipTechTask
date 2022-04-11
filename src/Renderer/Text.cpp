#include "Text.h"

#include "Renderer.h"

Text::Text(Renderer& renderer, const std::string& text, Font& font, SDL_Color color /*= WHITE_COLOR*/)
	: m_Text(text), m_Color(color), m_Font(font)
{
	SDL_Surface* surfaceMessage = TTF_RenderText_Solid(m_Font.GetTTFFont(), text.c_str(), m_Color);
	m_Texture = SDL_CreateTextureFromSurface(renderer.GetSDLRenderer(), surfaceMessage);
	m_RectSize = { surfaceMessage->w, surfaceMessage->h };
	SDL_FreeSurface(surfaceMessage);
}

Text::~Text()
{
	m_Font.Destroy();
	SDL_DestroyTexture(m_Texture);
}
