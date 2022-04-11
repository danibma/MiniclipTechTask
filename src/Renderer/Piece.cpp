#include "Piece.h"

#include "Renderer.h"

Piece::Piece(PieceColor color, int32_t spawnPositionX, int32_t spawnPositionY)
{
	m_Color		= color;
	m_PositionX = spawnPositionX;
	m_PositionY = spawnPositionY;
}

Piece::~Piece()
{
}
