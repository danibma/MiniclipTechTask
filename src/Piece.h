#pragma once

#include <SDL.h>

#include <utility>

#include "Renderer/Renderable.h"

#include "Sound/SoundEffect.h"

#include <memory>

#define PIECE_SIZE 32

#define ToRadians(x) (x * (M_PI / 180))

enum class PieceColor
{
	None,
	Green,
	LightBlue,
	Orange,
	Red
};

enum class PieceRotation
{
	Top,
	Left,
	Right,
	Down
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
	const PieceRotation GetRotation() const;

	inline bool IsLocked() const { return m_IsLocked; }

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
	bool IsCollidingVertically(int32_t limitY, int32_t limitHeight);

	/**
	 * Check if a piece is colliding with any type of limit horizontally
	 *
	 * @param int32_t limitX
	 * @param int32_t limitWidth
	 *
	 * @returns -1 if it can't move to the left, 1 if it can't move to the right and 0 if it isn't colliding with any limit
	 */
	int8_t IsCollidingHoriontally(int32_t limitX, int32_t limitWidth);

	/**
	 * Check if a piece is colliding with a piece vertically
	 *
	 * @param Piece& piece
	 *
	 * @returns 1 if it is colliding with any piece below it and 0 if it isn't colliding with piece
	 */
	int8_t IsCollidingWithPieceVertically(const Piece& piece);

	/**
	 * Check if a piece is colliding with a piece horizontally
	 *
	 * @param Piece& piece
	 *
	 * @returns -1 if it is colliding with any piece on the left, 1 if it is colliding with any piece on the right and 0 if it isn't colliding with any piece
	 */
	int8_t IsCollidingWithPieceHorizontally(const Piece& piece);

	inline void SetLocked(bool value) { m_IsLocked = value; }

	// Operator overloading
	friend bool operator==(const Piece& leftPiece, const Piece& rightPiece);
	friend bool operator!=(const Piece& leftPiece, const Piece& rightPiece);

private:
	PieceColor m_Color = PieceColor::None;
	bool m_IsLocked = false;
	int32_t m_CurrentRotation = 0;
};

