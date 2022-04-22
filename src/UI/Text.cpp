#include "Text.h"

#include "../Renderer/Renderer.h"

Text::Text(Renderer& renderer, const std::string& text, const Font& font, SDL_Color color /*= WHITE_COLOR*/)
	: m_Text(text), m_Font(font), m_Color(color)
{
	SDL_Surface* surfaceMessage = TTF_RenderText_Solid(m_Font.GetTTFFont(), m_Text.c_str(), m_Color);
	m_Texture = SDL_CreateTextureFromSurface(renderer.GetSDLRenderer(), surfaceMessage);
	m_RectSize = { surfaceMessage->w, surfaceMessage->h };
	m_Position = { 0, 0 };
	SDL_FreeSurface(surfaceMessage);
}

Text::~Text()
{
	SDL_DestroyTexture(m_Texture);
}

void Text::UpdateText(Renderer& renderer, const std::string& text)
{
	if (m_Texture != nullptr)
	{
		SDL_DestroyTexture(m_Texture);
		m_Texture = nullptr;
	}

	m_Text = text;

	SDL_Surface* surfaceMessage = TTF_RenderText_Solid(m_Font.GetTTFFont(), m_Text.c_str(), m_Color);
	m_Texture = SDL_CreateTextureFromSurface(renderer.GetSDLRenderer(), surfaceMessage);
	m_RectSize = { surfaceMessage->w, surfaceMessage->h };
	m_Position = { 0, 0 };
	SDL_FreeSurface(surfaceMessage);
}
