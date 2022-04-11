#pragma once

#include <SDL.h>
#include "Piece.h"

class Renderer
{
public:
	Renderer() : m_Renderer(nullptr) {};
	~Renderer();

	void Init(SDL_Window* window, uint32_t screenWidth, uint32_t screenHeight);

	void Clear();
	void Update();

	void DrawPiece(Piece& piece);
	void DrawGrid();
	void DrawScore();

	inline const int32_t& GetPieceSize() const { return m_PieceSize; }

private:
	SDL_Renderer* m_Renderer;

	// Pieces dimensions
	const int32_t m_PieceSize = 32;

	// Grid dimensions
	uint32_t m_GridWidth	= 0;
	uint32_t m_GridHeight	= 0;
	uint32_t m_GridX		= 0;
	uint32_t m_GridY		= 0;

	// pieces colors textures
	SDL_Texture* m_PieceGreen;
	SDL_Texture* m_PieceLightBlue;
	SDL_Texture* m_PieceOrange;
	SDL_Texture* m_PieceRed;
};

