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
	void DrawGrid();
	void DrawScore(Text& scoreText);
	void DrawText(Text& text, std::pair<int32_t, int32_t> position, std::pair<int32_t, int32_t> rectSize);
	void DrawText(Text& text, int32_t positionX, int32_t positionY, int32_t rectSizeX, int32_t rectSizeY);

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
	std::unordered_map<PieceColor, SDL_Texture*> m_Colors;
};

