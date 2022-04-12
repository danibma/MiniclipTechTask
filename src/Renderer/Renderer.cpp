#include "Renderer.h"
#include <stdio.h>


void Renderer::Init(SDL_Window* window, uint32_t screenWidth, uint32_t screenHeight)
{
	m_Renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (!m_Renderer)
		printf("Renderer could not be initialized: %s\n", SDL_GetError());

	if (TTF_Init() < 0)
		printf("SDL_TTF could not be initialized: %s\n", SDL_GetError());

	// Load colors respective texture
	SDL_Surface* surface = SDL_LoadBMP("assets/green.bmp");
	m_Colors[PieceColor::Green] = SDL_CreateTextureFromSurface(m_Renderer, surface);
	SDL_FreeSurface(surface);

	surface = SDL_LoadBMP("assets/lightblue.bmp");
	m_Colors[PieceColor::LightBlue] = SDL_CreateTextureFromSurface(m_Renderer, surface);
	SDL_FreeSurface(surface);

	surface = SDL_LoadBMP("assets/orange.bmp");
	m_Colors[PieceColor::Orange] = SDL_CreateTextureFromSurface(m_Renderer, surface);
	SDL_FreeSurface(surface);

	surface = SDL_LoadBMP("assets/red.bmp");
	m_Colors[PieceColor::Red] = SDL_CreateTextureFromSurface(m_Renderer, surface);
	SDL_FreeSurface(surface);
}

Renderer::~Renderer()
{
	for (const auto& texture : m_Colors)
	{
		SDL_DestroyTexture(texture.second);
	}

	m_Colors.clear();

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

void Renderer::DrawPiece(Piece& piece)
{
	auto pieceColor = piece.GetColor();
	auto [pieceX, pieceY] = piece.GetPosition();
	auto [sizeX, sizeY] = piece.GetSize();
	SDL_Rect rect = { pieceX, pieceY, sizeX, sizeY};

	SDL_RenderCopy(m_Renderer, m_Colors[pieceColor], nullptr, &rect);
}

void Renderer::DrawText(Text& text, std::pair<int32_t, int32_t> position)
{
	if (text.GetTexture())
	{
		auto [positionX, positionY] = position;
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
