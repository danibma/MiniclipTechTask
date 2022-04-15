#pragma once

#include <SDL.h>

#include <utility>

#include "Renderer/Renderable.h"

#include "Sound/SoundEffect.h"

#define PIECE_SIZE 32

enum class PieceColor : uint32_t
{
	None,
	Green,
	LightBlue,
	Orange,
	Red
};

namespace Utils
{
	inline PieceColor IntToPieceColor(uint32_t num)
	{
		switch (num)
		{
		case 1:
			return PieceColor::Green;
			break;
		case 2:
			return PieceColor::LightBlue;
			break;
		case 3:
			return PieceColor::Orange;
			break;
		case 4:
			return PieceColor::Red;
			break;
		default:
			return PieceColor::None;
			break;
		}
	}

	inline const char* PieceColorToString(PieceColor color)
	{
		switch (color)
		{
		case PieceColor::Green:
			return "Green";
			break;
		case PieceColor::LightBlue:
			return "LightBlue";
			break;
		case PieceColor::Orange:
			return "Orange";
			break;
		case PieceColor::Red:
			return "Red";
			break;
		default:
			return "None";
			break;
		}
	}
}

class Piece : public Renderable
{
public:
	Piece() {}
	Piece(PieceColor color, int32_t spawnPositionX, int32_t spawnPositionY);
	~Piece();

	inline const PieceColor& GetColor() const { return m_Color; }
	inline const int32_t GetRotation() const { return m_CurrentRotation; }

	inline const bool IsLocked() { return m_IsLocked; }

	void Rotate(int32_t angle, const std::pair<int32_t, int32_t>& centerPoint);

	/**
	 * Choose in which axis the piece will move
	 */
	void Move(float x, float y);

	/**
	 * Check if a piece is colliding with any type of limit vertically
	 *
	 * @param int32_t limitY
	 * @param int32_t limitHeight
	 *
	 * @returns true if it is colliding, false if it isn't
	 */
	int8_t IsCollidingVertically(int32_t limitY, int32_t limitHeight);

	/**
	 * Check if a piece is colliding with any type of limit horizontally
	 *
	 * @param int32_t limitX
	 * @param int32_t limitWidth
	 *
	 * @returns -1 if it can't move to the left, 1 if it can't move to the right and 0 if it isn't colliding with any limit
	 */
	int8_t IsCollidingHoriontally(int32_t limitX, int32_t limitWidth);
	bool IsCollidingWithPiece(Piece& piece);

	inline void SetLocked(bool value) { m_IsLocked = value; }

private:
	PieceColor m_Color = PieceColor::None;
	bool m_IsLocked = false;
	int32_t m_CurrentRotation = 0;
};

