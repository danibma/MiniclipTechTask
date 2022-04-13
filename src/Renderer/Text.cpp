#include "Text.h"

#include "Renderer.h"

Text::Text(Renderer& renderer, const std::string& text, const Font& font, SDL_Color color /*= WHITE_COLOR*/)
{
	SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font.GetTTFFont(), text.c_str(), color);
	m_Texture = SDL_CreateTextureFromSurface(renderer.GetSDLRenderer(), surfaceMessage);
	m_RectSize = { surfaceMessage->w, surfaceMessage->h };
	SDL_FreeSurface(surfaceMessage);
}

Text::~Text()
{
	SDL_DestroyTexture(m_Texture);
}
