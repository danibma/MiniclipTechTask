#pragma once

#include <string>

#include <SDL.h>

#include "Font.h"

#define WHITE_COLOR { 255, 255, 255, 255 }

class Renderer;

class Text
{
public:
	Text() = default;
	Text(Renderer& renderer, const std::string& text, const Font& font, SDL_Color color = WHITE_COLOR);
	~Text();

	inline SDL_Texture* GetTexture() const { return m_Texture; }
	inline const std::pair<int32_t, int32_t>& GetRectSize() const { return m_RectSize; }
	inline const std::pair<int32_t, int32_t>& GetPosition() const { return m_Position; }

	inline void SetTexture(SDL_Texture* texture) { m_Texture = texture; }
	inline void SetRectSize(int32_t width, int32_t height) { m_RectSize = { width, height }; }
	inline void SetPosition(int32_t posX, int32_t posY) { m_Position = { posX, posY }; }

private:
	std::pair<int32_t, int32_t> m_RectSize;
	std::pair<int32_t, int32_t> m_Position;

	SDL_Texture* m_Texture = nullptr;
};

