#include <iostream>

#include <SDL.h>
#include <SDL_mixer.h>

#include "Renderer/Renderer.h"
#include "Renderer/Piece.h"
#include "Sound/Music.h"
#include "Sound/SoundEffect.h"

// Game Requirements
// - The game is played on a 8x16 grid -
// - Objective of the game is to match same colored pieces
// - There should be 4 different colored pieces -
// - Forming groups of 4 + pieces, in L, T, square or other fully connecting shapes destroys the pieces
// - Pieces always appear in pairs, each piece may randomly share or differ in color to the other piece in the pair
// - Pairs are spawned above the top of the grid, dropping down until they are placed
// - The pair is considered placed when any of the pieces of the pair cannot be moved further down
// - The game is lost if it is not possible to place the pair within the grid
// - The player controls the pair�s movement by :
//		- Moving the pair sideways
//		- Rotating the pair in 90 degree angles(left or right)
//		- Making the pair fall faster
// - Once the pair is placed
// - The player can no longer move the pair
// - The pieces will unpair and each of them will fall to the lowest position it can reach
// - Once there is no movement(all pieces placed), matches are validatedand removed from the grid
// - The next pair will be spawned once all matches are cleared
// - Check the following link for reference: youtube.com/watch?v=YJjRJ_4gcUw

//Screen dimension constants
constexpr uint32_t SCREEN_WIDTH = 960;
constexpr uint32_t SCREEN_HEIGHT = 720;

static bool s_IsRunning		= true;
static bool s_MusicEnabled	= true;

Renderer renderer;

int main(int argc, char* args[])
{
	//The window we'll be rendering to
	SDL_Window* window = nullptr;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0)
		printf("SDL could not be initialized: %s\n", SDL_GetError());

	//Create window
	window = SDL_CreateWindow("Miniclip Tech Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
	if (!window)
	{
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		return 0;
	}

	//Initialize SDL_mixer 
	if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1)
	{
		printf("SDL_mixer could not be initialized: %s\n", SDL_GetError());
		return 1;
	}

	Music backgroundMusic("assets/sounds/music/background_music.mp3");
	Music GameOverMusic("assets/sounds/music/gameover.mp3");
	SoundEffect PieceMoveSound("assets/sounds/sound_effects/piece_move.mp3");
	SoundEffect PieceDropSound("assets/sounds/sound_effects/piece_drop.mp3");
	SoundEffect PieceGroupRemoveSound("assets/sounds/sound_effects/piece_group_remove.mp3");

	if (s_MusicEnabled)
		backgroundMusic.Play(true);


	// Initialize Renderer
	renderer.Init(window, SCREEN_WIDTH, SCREEN_HEIGHT);

	Piece greenPiece(PieceColor::Green, 50, 50);
	Piece lightBluePiece(PieceColor::LightBlue, 100, 100);
	Piece orangePiece(PieceColor::Orange, 150, 150);
	Piece redPiece(PieceColor::Red, 200, 200);

	while (s_IsRunning)
	{
		SDL_Event event;
		if (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
				s_IsRunning = false;

			if (event.type == SDL_MOUSEBUTTONDOWN)
				printf("TODO: mouse click!\n");

			if (event.type == SDL_KEYDOWN)
			{
				// TODO(Daniel): Make an option on main menu, thats called "Controls" and show this controls
				// Keybinds
				if (event.key.keysym.sym == SDLK_a || event.key.keysym.sym == SDLK_LEFT)
					printf("todo: move left key pressed!\n");
				else if (event.key.keysym.sym == SDLK_d || event.key.keysym.sym == SDLK_RIGHT)
					printf("todo: move right key pressed!\n");
				else if (event.key.keysym.sym == SDLK_s || event.key.keysym.sym == SDLK_DOWN)
					printf("todo: move down key pressed!\n");
				else if (event.key.keysym.sym == SDLK_z)
					printf("todo: rotate left key pressed!\n");
				else if (event.key.keysym.sym == SDLK_x)
					printf("todo: rotate right key pressed!\n");

				GameOverMusic.Play(false);
			}
		}

		// Clear screen
		renderer.Clear();

		// Draw
		// Draw Grid
		renderer.DrawGrid();

		// Draw score lines
		renderer.DrawScore();

		// Draw piece
		renderer.DrawPiece(greenPiece);
		renderer.DrawPiece(lightBluePiece);
		renderer.DrawPiece(orangePiece);
		renderer.DrawPiece(redPiece);

		// Present
		renderer.Update();
	}
	
	//Destroy window
	SDL_DestroyWindow(window);

	//Quit SDL subsystems
	SDL_Quit();

    return 0;
}