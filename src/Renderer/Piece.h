#pragma once

#include <SDL.h>

#include <utility>

#define PIECE_SIZE 32

enum class PieceColor
{
	None,
	Green,
	LightBlue,
	Orange,
	Red
};

class Piece
{
public:
	Piece() {}
	Piece(PieceColor color, int32_t spawnPositionX, int32_t spawnPositionY);
	~Piece();

	inline const PieceColor& GetColor() const { return m_Color; }
	inline const std::pair<int32_t, int32_t> GetPosition() const { return { m_PositionX, m_PositionY }; }
	inline const std::pair<int32_t, int32_t> GetSize() const { return { m_Width, m_Height }; }

	void SetPosition(int32_t x, int32_t y);
	void SetSize(int32_t x, int32_t y);

	void Rotate(float angle, const int32_t centerX, const int32_t centerY);
	void Rotate(float angle, const std::pair<int32_t, int32_t>& centerPoint);

	// Choose in which axis the piece will move
	void Move(float x, float y);

	bool IsCollidingWithPiece(Piece& piece);
	bool IsCollidingWithGrid(int32_t x, int32_t y, int32_t width, int32_t height);

private:
	PieceColor m_Color = PieceColor::None;

	int32_t m_PositionX = 0;
	int32_t m_PositionY = 0;
	uint32_t m_Width = 0;
	uint32_t m_Height = 0;
};

