#pragma once

#include "../Renderer/Renderable.h"
#include "Text.h"

#include <functional>

class Button : public Renderable
{
public:
	Button(Text& text, SDL_Texture* backgroundTexture, SDL_Texture* pressedTexture, std::pair<int32_t, int32_t> position, std::pair<int32_t, int32_t> size);
	~Button();

	inline Text& GetText() { return m_Text; }

	void SetOnClickCallback(const std::function<void()>& func);

	void OnPressed();

	void OnRelease();

	bool IsMouseOver();

private:
	Text m_Text;

	std::function<void()> m_ClickCallback = nullptr;

	bool m_IsPressed = false;

	SDL_Texture* m_BackgroundTexture;
	SDL_Texture* m_PressedTexture;
};

