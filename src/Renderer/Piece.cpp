#include "Piece.h"

#include "Renderer.h"

#define _USE_MATH_DEFINES
#include <math.h>

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

void Piece::RotateLeft()
{
	float angle = ToRadians(90.0f);

	int32_t centerX = 250.0f;
	int32_t centerY = 250.0f;

	auto x = cos(angle) * (m_PositionX - centerX) - sin(angle) * (m_PositionY - centerY) + centerX;
	auto y = sin(angle) * (m_PositionX - centerX) + cos(angle) * (m_PositionY - centerY) + centerY;

	m_PositionX = x;
	m_PositionY = y;
}

void Piece::RotateRight()
{
	float angle = ToRadians(-90.0f);

	int32_t centerX = 250.0f;
	int32_t centerY = 250.0f;

	auto x = cos(angle) * (m_PositionX - centerX) - sin(angle) * (m_PositionY - centerY) + centerX;
	auto y = sin(angle) * (m_PositionX - centerX) + cos(angle) * (m_PositionY - centerY) + centerY;

	m_PositionX = x;
	m_PositionY = y;
}
