#include "Renderable.h"

#include <stdio.h>

Renderable::Renderable(int32_t positionX, int32_t positionY, int32_t sizeX, int32_t sizeY, const std::string& textureName)
	: m_TextureName(textureName)
{
	m_PositionX = positionX;
	m_PositionY = positionY;
	m_Width = sizeX;
	m_Height = sizeY;
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

void Renderable::SetTextureName(const std::string& texture)
{
	m_TextureName = texture;
}