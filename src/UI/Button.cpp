#include "Button.h"

Button::Button(Text& text, const std::string& backgroundTextureName, const std::string& pressedTextureName, std::pair<int32_t, int32_t> position, std::pair<int32_t, int32_t> size)
	: m_Text(text)
{
	auto [posX, posY] = position;
	auto [sizeX, sizeY] = size;

	m_PositionX = posX;
	m_PositionY = posY;
	m_Width = sizeX;
	m_Height = sizeY;

	m_TextureName = backgroundTextureName;
	m_BackgroundTextureName = backgroundTextureName;
	m_PressedTextureName = pressedTextureName;

	// Initiliaze the callback as an empty function
	m_ClickCallback = []() { };

	// Set text position in the middle
	int32_t buttonPosX = posX + ((sizeX / 2) - (m_Text.GetRectSize().first / 2));
	int32_t buttonPosY = posY + ((sizeY / 2) - (m_Text.GetRectSize().second / 2));
	m_Text.SetPosition(buttonPosX, buttonPosY);
}

Button::~Button()
{
}

void Button::SetOnClickCallback(const std::function<void()>& func)
{
	m_ClickCallback = func;
}

void Button::OnPressed()
{
	m_TextureName = m_PressedTextureName;
	m_IsPressed = true;
}

void Button::OnRelease()
{
	if (m_IsPressed)
	{
		m_TextureName = m_BackgroundTextureName;

		m_ClickCallback();

		m_IsPressed = false;
	}
}

bool Button::IsMouseOver()
{
	//Check if mouse
	int mouseX, mouseY;
	SDL_GetMouseState(&mouseX, &mouseY);

	return	(mouseX >= m_PositionX &&
			 mouseX <= m_PositionX + m_Width &&
			 mouseY >= m_PositionY &&
			 mouseY <= m_PositionY + m_Height);
}
