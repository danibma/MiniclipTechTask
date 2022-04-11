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

void Piece::RotateLeft()
{
	m_PositionX -= Renderer::GetPieceSize();
	m_PositionY -= Renderer::GetPieceSize();
}

void Piece::RotateRight()
{
	m_PositionX += Renderer::GetPieceSize();
	m_PositionY += Renderer::GetPieceSize();
}
