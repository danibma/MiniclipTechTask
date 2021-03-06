#include "Renderer.h"
#include <stdio.h>


void Renderer::Init(SDL_Window* window, uint32_t screenWidth, uint32_t screenHeight)
{
	m_Renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (!m_Renderer)
		printf("Renderer could not be initialized: %s\n", SDL_GetError());

	if (TTF_Init() < 0)
		printf("SDL_TTF could not be initialized: %s\n", SDL_GetError());

	SDL_SetRenderDrawBlendMode(m_Renderer, SDL_BLENDMODE_BLEND);
}

Renderer::~Renderer()
{
	for (auto& texture : m_TextureCache)
		SDL_DestroyTexture(texture.second);

	m_TextureCache.clear();

	SDL_DestroyRenderer(m_Renderer);

	TTF_Quit();
}

void Renderer::Clear()
{
	// Clear the screen with the black color
	SDL_SetRenderDrawColor(m_Renderer, 0, 0, 0, 255);
	SDL_RenderClear(m_Renderer);
}

void Renderer::Update()
{
	SDL_RenderPresent(m_Renderer);
}

void Renderer::AddTexture(const std::string& textureName, const std::string& path)
{
	SDL_Surface* surface = SDL_LoadBMP(path.c_str());
	auto texture = SDL_CreateTextureFromSurface(m_Renderer, surface);
	SDL_FreeSurface(surface);

	m_TextureCache[textureName] = texture;
}

void Renderer::DrawRenderable(Renderable& piece)
{
	auto [pieceX, pieceY] = piece.GetPosition();
	auto [sizeX, sizeY] = piece.GetSize();
	SDL_Rect rect = { pieceX, pieceY, sizeX, sizeY};

	SDL_RenderCopy(m_Renderer, GetTexture(piece.GetTextureName()), nullptr, &rect);
}

void Renderer::DrawButton(Button& button)
{
	// Draw button background
	auto [posX, posY] = button.GetPosition();
	auto [sizeX, sizeY] = button.GetSize();
	SDL_Rect rect = { posX, posY, sizeX, sizeY };

	SDL_RenderCopy(m_Renderer, GetTexture(button.GetTextureName()), nullptr, &rect);

	// Draw button text
	DrawText(button.GetText());
}

void Renderer::DrawText(Text& text)
{
	if (text.GetTexture())
	{
		auto [positionX, positionY] = text.GetPosition();
		auto [sizeX, sizeY] = text.GetRectSize();
		SDL_Rect rect = { positionX, positionY, sizeX, sizeY };

		SDL_RenderCopy(m_Renderer, text.GetTexture(), nullptr, &rect);
	}
	else
	{
		printf("Text texture is null, dont forget to create the font texture!");
	}
}

void Renderer::DrawText(Text& text, int32_t positionX, int32_t positionY)
{
	if (text.GetTexture())
	{
		auto [sizeX, sizeY] = text.GetRectSize();
		SDL_Rect rect = { positionX, positionY, sizeX, sizeY };

		SDL_RenderCopy(m_Renderer, text.GetTexture(), nullptr, &rect);
	}
	else
	{
		printf("Text texture is null, dont forget to create the font texture!");
	}
}
