#pragma once

#include <SDL.h>

#include <utility>

enum class PieceColor
{
	Green,
	LightBlue,
	Orange,
	Red
};

class Piece
{
public:
	Piece(PieceColor color, int32_t spawnPositionX, int32_t spawnPositionY);
	~Piece();

	inline const PieceColor& GetColor() const { return m_Color; }
	inline const std::pair<int32_t, int32_t> GetPosition() const { return { m_PositionX, m_PositionY }; }

private:
	PieceColor m_Color;
	int32_t m_PositionX = 0;
	int32_t m_PositionY = 0;
};

