#include "Renderable.h"

#include <stdio.h>

Renderable::Renderable(int32_t positionX, int32_t positionY, int32_t sizeX, int32_t sizeY, SDL_Texture* texture)
{
	m_PositionX = positionX;
	m_PositionY = positionY;
	m_Width = sizeX;
	m_Height = sizeY;
	m_Texture = texture;
}

void Renderable::SetPosition(int32_t x, int32_t y)
{
	m_PositionX = x;
	m_PositionY = y;
}

void Renderable::SetSize(int32_t x, int32_t y)
{
	m_Width = x;
	m_Height = y;
}

void Renderable::SetTexture(SDL_Texture* texture)
{
	m_Texture = texture;
}