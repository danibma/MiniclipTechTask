#pragma once

#include "../Renderer/Renderable.h"
#include "Text.h"

#include <functional>

class Button : public Renderable
{
public:
	Button(Text& text, const std::string& backgroundTextureName, const std::string& pressedTextureName, std::pair<int32_t, int32_t> position, std::pair<int32_t, int32_t> size);
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

	std::string m_BackgroundTextureName;
	std::string m_PressedTextureName;
};

