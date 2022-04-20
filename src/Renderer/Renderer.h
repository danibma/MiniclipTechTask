#pragma once

#include <unordered_map>

#include <SDL.h>
#include <SDL_ttf.h>

#include "Renderable.h"
#include "../UI/Text.h"
#include "../UI/Button.h"

class Renderer
{
public:
	Renderer() : m_Renderer(nullptr) {};
	~Renderer();

	void Init(SDL_Window* window, uint32_t screenWidth, uint32_t screenHeight);

	void Clear();
	void Update();

	SDL_Texture* CreateTexture(const char* path);

	void DrawRenderable(std::shared_ptr<Renderable> piece);
	void DrawButton(std::shared_ptr<Button> button);
	void DrawText(Text& text);
	void DrawText(Text& text, int32_t positionX, int32_t positionY);

	inline SDL_Renderer* GetSDLRenderer() const { return m_Renderer; }

private:
	SDL_Renderer* m_Renderer;
};

