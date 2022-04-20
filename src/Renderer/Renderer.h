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

	/**
	 * Loads a texture from the given path, adds it to the texture cache with key being the given texture name
	 *
	 * @param const std::string& textureName
	 * @param const std::string& path
	 */
	void AddTexture(const std::string& textureName, const std::string& path);

	void DrawRenderable(Renderable& piece);
	void DrawButton(Button& button);
	void DrawText(Text& text);
	void DrawText(Text& text, int32_t positionX, int32_t positionY);

	inline SDL_Renderer* GetSDLRenderer() const { return m_Renderer; }
	inline SDL_Texture* GetTexture(std::string_view textureName) { return m_TextureCache[textureName.data()]; }

private:
	SDL_Renderer* m_Renderer;
	std::unordered_map<std::string, SDL_Texture*> m_TextureCache;
};

