#pragma once

#include <utility>

#include <SDL.h>

class Renderable
{
public:
	Renderable(int32_t positionX, int32_t positionY, int32_t sizeX, int32_t sizeY, SDL_Texture* texture);
	Renderable() = default;
	virtual ~Renderable() = default;

	inline const std::pair<int32_t, int32_t> GetPosition() const { return { m_PositionX, m_PositionY }; }
	inline const std::pair<int32_t, int32_t> GetSize() const { return { m_Width, m_Height }; }
	inline SDL_Texture* GetTexture() { return m_Texture; }

	void SetPosition(int32_t x, int32_t y);
	void SetSize(int32_t x, int32_t y);
	void SetTexture(SDL_Texture* texture);

protected:
	int32_t m_PositionX = 0;
	int32_t m_PositionY = 0;
	int32_t m_Width = 0;
	int32_t m_Height = 0;

	// We dont need to destroy this texture on the destructor,
	// because every texture on the main program is getting cached
	// so its destroyed at the end of the game
	SDL_Texture* m_Texture = nullptr;
};

