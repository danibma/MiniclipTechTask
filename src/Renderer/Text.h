#pragma once

#include <string>

#include <SDL.h>

#include "Font.h"

#define BLACK_COLOR {   0,   0,   0, 255 }
#define WHITE_COLOR { 255, 255, 255, 255 }

class Text
{
public:
	Text(const std::string& text, Font& font, SDL_Color color = WHITE_COLOR);
	~Text();

	inline const std::string& GetText() const { return m_Text; }
	inline const SDL_Color& GetColor() const { return m_Color; }
	inline const Font& GetFont() const { return m_Font; }
	inline SDL_Texture* GetTexture() const { return m_Texture; }

	void SetTexture(SDL_Texture* texture) { m_Texture = texture; }
	void SetText(const std::string& text) { m_Text = text; }
private:
	std::string m_Text;
	SDL_Color m_Color;
	Font m_Font;
	SDL_Texture* m_Texture = nullptr;
};

