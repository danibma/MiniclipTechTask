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
	m_MoveSound = SoundEffect("assets/sounds/sound_effects/piece_move.mp3");
}

Piece::~Piece()
{
	m_MoveSound.Destroy();
}

void Piece::Rotate(int32_t angle, const std::pair<int32_t, int32_t>& centerPoint)
{
	if (!m_IsLocked)
	{
		m_CurrentRotation += angle;
		if (m_CurrentRotation % 360 == 0)
			m_CurrentRotation = 0;

		double angleInRadians = ToRadians(angle);

		auto [centerX, centerY] = centerPoint;

		double x = cos(angleInRadians) * (m_PositionX - centerX) - sin(angleInRadians) * (m_PositionY - centerY) + centerX;
		double y = sin(angleInRadians) * (m_PositionX - centerX) + cos(angleInRadians) * (m_PositionY - centerY) + centerY;

		m_PositionX = (int32_t)x;
		m_PositionY = (int32_t)y;

		m_MoveSound.Play();
	}
}

void Piece::Move(float x, float y)
{
	if (!m_IsLocked)
	{
		x = std::clamp(x, -1.0f, 1.0f);

		m_PositionX += ((int32_t)x * PIECE_SIZE);
		m_PositionY += ((int32_t)y * PIECE_SIZE);

		m_MoveSound.Play();
	}
}

int8_t Piece::IsCollidingVertically(int32_t limitY, int32_t limitHeight)
{
	if (m_PositionY >= (limitY + limitHeight) - PIECE_SIZE)
	{
		return true;
	}

	return false;
}

int8_t Piece::IsCollidingHoriontally(int32_t limitX, int32_t limitWidth)
{
	if (m_PositionX >= (limitX + limitWidth) - PIECE_SIZE)
		return 1;

	if (m_PositionX < limitX + PIECE_SIZE)
		return -1;

	return 0;
}

bool Piece::IsCollidingWithPiece(Piece& piece)
{
	auto [positionX, positionY] = piece.GetPosition();
	auto [sizeX, sizeY] = piece.GetSize();

	if ((m_PositionX + m_Width) <= (positionX) || (m_PositionX) >= (positionX + sizeX)) return false;
	if ((m_PositionY + m_Height) < (positionY) || (m_PositionY) > (positionY + sizeY)) return false;

	return true;
}
