#pragma once

#include <unordered_map>

#include <SDL.h>
#include <SDL_ttf.h>

#include "Piece.h"
#include "Text.h"

class Renderer
{
public:
	Renderer() : m_Renderer(nullptr) {};
	~Renderer();

	void Init(SDL_Window* window, uint32_t screenWidth, uint32_t screenHeight);

	void Clear();
	void Update();

	void DrawPiece(Piece& piece);
	void DrawText(Text& text, std::pair<int32_t, int32_t> position);
	void DrawText(Text& text, int32_t positionX, int32_t positionY);

	inline SDL_Renderer* GetSDLRenderer() const { return m_Renderer; }

private:
	SDL_Renderer* m_Renderer;

	// pieces colors textures
	std::unordered_map<PieceColor, SDL_Texture*> m_Colors;
};

