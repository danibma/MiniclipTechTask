#pragma once

#include <string>

#include <SDL.h>

#include "Font.h"

#define WHITE_COLOR { 255, 255, 255, 255 }

class Renderer;

class Text
{
public:
	Text(Renderer& renderer, const std::string& text, Font& font, SDL_Color color = WHITE_COLOR);
	~Text();

	inline const std::string& GetText() const { return m_Text; }
	inline const SDL_Color& GetColor() const { return m_Color; }
	inline const Font& GetFont() const { return m_Font; }
	inline SDL_Texture* GetTexture() const { return m_Texture; }
	inline const std::pair<int32_t, int32_t>& GetRectSize() const { return m_RectSize; }

	inline void SetTexture(SDL_Texture* texture) { m_Texture = texture; }
	inline void SetText(const std::string& text) { m_Text = text; }
	inline void SetRectSize(int32_t width, int32_t height) { m_RectSize = { width, height }; }

private:
	std::string m_Text;
	SDL_Color m_Color;
	Font m_Font;
	std::pair<int32_t, int32_t> m_RectSize;

	SDL_Texture* m_Texture = nullptr;
};

