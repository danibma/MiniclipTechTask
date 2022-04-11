#include "Piece.h"

#include "Renderer.h"

#include <algorithm>

#define ToRadians(x) (x * (M_PI / 180))

Piece::Piece(PieceColor color, int32_t spawnPositionX, int32_t spawnPositionY)
{
	m_Color		= color;
	m_PositionX = spawnPositionX;
	m_PositionY = spawnPositionY;
}

Piece::~Piece()
{
}

void Piece::Rotate(float angle, const int32_t centerX, const int32_t centerY)
{
	float angleInRadians = ToRadians(angle);

	float x = cos(angleInRadians) * (m_PositionX - centerX) - sin(angleInRadians) * (m_PositionY - centerY) + centerX;
	float y = sin(angleInRadians) * (m_PositionX - centerX) + cos(angleInRadians) * (m_PositionY - centerY) + centerY;

	m_PositionX = x;
	m_PositionY = y;
}

void Piece::Rotate(float angle, const std::pair<int32_t, int32_t>& centerPoint)
{
	float angleInRadians = ToRadians(angle);

	auto [centerX, centerY] = centerPoint;

	float x = cos(angleInRadians) * (m_PositionX - centerX) - sin(angleInRadians) * (m_PositionY - centerY) + centerX;
	float y = sin(angleInRadians) * (m_PositionX - centerX) + cos(angleInRadians) * (m_PositionY - centerY) + centerY;

	m_PositionX = x;
	m_PositionY = y;
}

void Piece::Move(float x, float y)
{
	x = std::clamp(x, -1.0f, 1.0f);

	m_PositionX += (x * Renderer::GetPieceSize());
	m_PositionY += (y * Renderer::GetPieceSize());
}
