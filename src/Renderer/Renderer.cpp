#include "Renderer.h"
#include <stdio.h>

void Renderer::Init(SDL_Window* window, uint32_t screenWidth, uint32_t screenHeight)
{
	m_Renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (!m_Renderer)
		printf("Renderer could not be initialized: %s\n", SDL_GetError());

	if (TTF_Init() < 0)
		printf("SDL_TTF could not be initialized: %s\n", SDL_GetError());

	// Grid dimensions
	m_GridWidth		=  8 * m_PieceSize;
	m_GridHeight	= 16 * m_PieceSize;
	m_GridX			= (screenWidth / 2) -  (m_GridWidth / 2);
	m_GridY			= (screenHeight / 2) - (m_GridHeight / 2);

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
	SDL_DestroyRenderer(m_Renderer);
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
	auto pieceColor			= piece.GetColor();
	auto [pieceX, pieceY]	= piece.GetPosition();
	SDL_Rect rect			= { pieceX, pieceY, m_PieceSize, m_PieceSize };

	SDL_RenderCopy(m_Renderer, m_Colors[pieceColor], nullptr, &rect);
}

void Renderer::DrawGrid()
{
	SDL_SetRenderDrawColor(m_Renderer, 100, 100, 100, SDL_ALPHA_OPAQUE);
	SDL_RenderDrawLine(m_Renderer, m_GridX, m_GridY, m_GridX, (m_GridY + m_GridHeight)); // Left Line
	SDL_RenderDrawLine(m_Renderer, m_GridX, m_GridY, (m_GridX + m_GridWidth), m_GridY); // Top Line
	SDL_RenderDrawLine(m_Renderer, (m_GridX + m_GridWidth), m_GridY, (m_GridX + m_GridWidth), (m_GridY + m_GridHeight)); // Right Line
	SDL_RenderDrawLine(m_Renderer, m_GridX, (m_GridY + m_GridHeight), (m_GridX + m_GridWidth), (m_GridY + m_GridHeight)); // Bottom Line
}

void Renderer::DrawScore(Text& scoreText)
{
	SDL_RenderDrawLine(m_Renderer, 10, m_GridY, (10 + 250), m_GridY); // Left Line
	SDL_RenderDrawLine(m_Renderer, 10, m_GridY, 10, (m_GridY + 100)); // Top Line
	SDL_RenderDrawLine(m_Renderer, (10 + 250), m_GridY, (10 + 250), (m_GridY + 100)); // Right Line
	SDL_RenderDrawLine(m_Renderer, 10, (m_GridY + 100), (10 + 250), (m_GridY + 100)); // Bottom Line

	DrawText(scoreText, { 20, m_GridY + 20 }, { m_GridY + 100, m_GridY / 2 });
}

void Renderer::DrawText(Text& text, std::pair<int32_t, int32_t> position, std::pair<int32_t, int32_t> rectSize)
{
	SDL_Surface* surfaceMessage = TTF_RenderText_Solid(text.GetFont().GetTTFFont(), text.GetText().c_str(), text.GetColor());
	auto fontTexture = SDL_CreateTextureFromSurface(m_Renderer, surfaceMessage);
	text.SetTexture(fontTexture);
	SDL_FreeSurface(surfaceMessage);

	auto [positionX, positionY] = position;
	auto [rectX, rectY] = rectSize;
	SDL_Rect rect = { positionX, positionY, rectX, rectY };

	SDL_RenderCopy(m_Renderer, text.GetTexture(), nullptr, &rect);
}

void Renderer::DrawText(Text& text, int32_t positionX, int32_t positionY, int32_t rectSizeX, int32_t rectSizeY)
{
	SDL_Surface* surfaceMessage = TTF_RenderText_Solid(text.GetFont().GetTTFFont(), text.GetText().c_str(), text.GetColor());
	auto fontTexture = SDL_CreateTextureFromSurface(m_Renderer, surfaceMessage);
	text.SetTexture(fontTexture);
	SDL_FreeSurface(surfaceMessage);

	SDL_Rect rect = { positionX, positionY, rectSizeX, rectSizeY };

	SDL_RenderCopy(m_Renderer, text.GetTexture(), nullptr, &rect);
}
