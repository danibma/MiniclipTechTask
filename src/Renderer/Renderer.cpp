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

	// Load main menu font and in game font
	// it is the same font, but with different sizes
	// these are going to be the most used so we load them up before runtime
	m_MainMenuFont = TTF_OpenFont("assets/Fonts/Teletoon.ttf", 30);
	m_InGameFont = TTF_OpenFont("assets/Fonts/Teletoon.ttf", 20);
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
	auto pieceColor = piece.GetColor();
	auto [pieceX, pieceY] = piece.GetPosition();
	SDL_Rect rect = { pieceX, pieceY, m_PieceSize, m_PieceSize };

	SDL_RenderCopy(m_Renderer, m_Colors[pieceColor], nullptr, &rect);
}

void Renderer::DrawGrid()
{
	SDL_SetRenderDrawColor(m_Renderer, 100, 100, 100, SDL_ALPHA_OPAQUE);
	SDL_RenderDrawLine(m_Renderer, m_GridX, m_GridY, m_GridX, (m_GridY + m_GridHeight));
	SDL_RenderDrawLine(m_Renderer, m_GridX, m_GridY, (m_GridX + m_GridWidth), m_GridY);
	SDL_RenderDrawLine(m_Renderer, (m_GridX + m_GridWidth), m_GridY, (m_GridX + m_GridWidth), (m_GridY + m_GridHeight));
	SDL_RenderDrawLine(m_Renderer, m_GridX, (m_GridY + m_GridHeight), (m_GridX + m_GridWidth), (m_GridY + m_GridHeight));
}

void Renderer::DrawScore()
{
	SDL_RenderDrawLine(m_Renderer, 10, m_GridY, (10 + 250), m_GridY);
	SDL_RenderDrawLine(m_Renderer, 10, m_GridY, 10, (m_GridY + 100));
	SDL_RenderDrawLine(m_Renderer, (10 + 250), m_GridY, (10 + 250), (m_GridY + 100));
	SDL_RenderDrawLine(m_Renderer, 10, (m_GridY + 100), (10 + 250), (m_GridY + 100));
}
