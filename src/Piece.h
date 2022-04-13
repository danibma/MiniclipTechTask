#pragma once

#include <SDL.h>

#include <utility>

#include "Renderer/Renderable.h"

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

	void Rotate(float angle, const int32_t centerX, const int32_t centerY);
	void Rotate(float angle, const std::pair<int32_t, int32_t>& centerPoint);

	// Choose in which axis the piece will move
	void Move(float x, float y);

	bool IsCollidingWithPiece(Piece& piece);

private:
	PieceColor m_Color = PieceColor::None;

};

