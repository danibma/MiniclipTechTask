#include "Piece.h"

#include <algorithm>

#define ToRadians(x) (x * (M_PI / 180))

Piece::Piece(PieceColor color, int32_t spawnPositionX, int32_t spawnPositionY)
{
	m_Color		= color;
	m_PositionX = spawnPositionX;
	m_PositionY = spawnPositionY;
	m_Width = PIECE_SIZE;
	m_Height = PIECE_SIZE;
}

Piece::~Piece()
{
}

void Piece::SetPosition(int32_t x, int32_t y)
{
	m_PositionX = x;
	m_PositionY = y;
}

void Piece::SetSize(int32_t x, int32_t y)
{
	m_Width = x;
	m_Height = y;
}

void Piece::Rotate(float angle, const int32_t centerX, const int32_t centerY)
{
	float angleInRadians = ToRadians(angle);

	float x = cos(angleInRadians) * (m_PositionX - centerX) - sin(angleInRadians) * (m_PositionY - centerY) + centerX;
	float y = sin(angleInRadians) * (m_PositionX - centerX) + cos(angleInRadians) * (m_PositionY - centerY) + centerY;

	m_PositionX = (int32_t)x;
	m_PositionY = (int32_t)y;
}

void Piece::Rotate(float angle, const std::pair<int32_t, int32_t>& centerPoint)
{
	float angleInRadians = ToRadians(angle);

	auto [centerX, centerY] = centerPoint;

	float x = cos(angleInRadians) * (m_PositionX - centerX) - sin(angleInRadians) * (m_PositionY - centerY) + centerX;
	float y = sin(angleInRadians) * (m_PositionX - centerX) + cos(angleInRadians) * (m_PositionY - centerY) + centerY;

	m_PositionX = (int32_t)x;
	m_PositionY = (int32_t)y;
}

void Piece::Move(float x, float y)
{
	x = std::clamp(x, -1.0f, 1.0f);

	m_PositionX += ((int32_t)x * PIECE_SIZE);
	m_PositionY += ((int32_t)y * PIECE_SIZE);
}

bool Piece::IsCollidingWithPiece(Piece& piece)
{
	auto [positionX, positionY] = piece.GetPosition();

	if ((m_PositionX + PIECE_SIZE) == positionX ||
		(m_PositionY + PIECE_SIZE) == positionY ||
		(positionX + PIECE_SIZE) == m_PositionX ||
		(positionY + PIECE_SIZE) == m_PositionY)
	{
		printf("Is Collinding");
		return true;
	}

	return false;
}
