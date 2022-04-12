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
// - The player controls the pair’s movement by :
//		- Moving the pair sideways -
//		- Rotating the pair in 90 degree angles(left or right) -
//		- Making the pair fall faster -
// - Once the pair is placed:
//		- The player can no longer move the pair
//		- The pieces will unpair and each of them will fall to the lowest position it can reach
//		- Once there is no movement(all pieces placed), matches are validatedand removed from the grid
// - The next pair will be spawned once all matches are cleared
// - Check the following link for reference: youtube.com/watch?v=YJjRJ_4gcUw

#define MAX_FRAMERATE 60

//Screen dimension constants
constexpr uint32_t SCREEN_WIDTH = 960;
constexpr uint32_t SCREEN_HEIGHT = 720;

static bool s_IsRunning		= true;
static bool s_MusicEnabled	= false;

Renderer renderer;

std::vector<Piece> pieces;
std::unordered_map<const char*, Piece> spawnPieces;

void SpawnNewPair()
{

}

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

	Piece orangePiece(PieceColor::Orange, 384, 104);
	Piece redPiece(PieceColor::Red, 384 - PIECE_SIZE, 104);

	spawnPieces["right"] = orangePiece;
	spawnPieces["left"] = redPiece;

	Font teletoonInGame("assets/Fonts/Teletoon.ttf", 42);
	Text scoreText(renderer, "Score: 0", teletoonInGame);

	//Timer
	float lastFrame = 0;
	float time;
	float timestep;
	float maxPeriod = (float)1000 / MAX_FRAMERATE;
	int timeInGame = 0;

	// Spawn the first pair of pieces
	SpawnNewPair();

	// Grid Values
	// Grid dimensions
	auto gridWidth = 8 * PIECE_SIZE;
	auto gridHeight = 16 * PIECE_SIZE;
	auto gridPositionX = (SCREEN_WIDTH / 2) - (gridWidth / 2);
	auto gridPositionY = (SCREEN_HEIGHT / 2) - (gridHeight / 2);

	// This way the pieces don't stay on top of the lines
	gridPositionX--;
	gridPositionY--;
	gridWidth++;
	gridHeight++;

	while (s_IsRunning)
	{
		time = (float)SDL_GetTicks();
		timestep = time - lastFrame;

		if (timestep >= maxPeriod)
		{
			// NOTE(Daniel): Dont run this while the game is over and the game is on main menu
			timeInGame++;

			lastFrame = time;

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
					{
						PieceMoveSound.Play();
						spawnPieces["left"].Move(-1, 0);
						spawnPieces["right"].Move(-1, 0);
					}
					else if (event.key.keysym.sym == SDLK_d || event.key.keysym.sym == SDLK_RIGHT)
					{
						PieceMoveSound.Play();
						spawnPieces["left"].Move(1, 0);
						spawnPieces["right"].Move(1, 0);
					}
					else if (event.key.keysym.sym == SDLK_s || event.key.keysym.sym == SDLK_DOWN)
					{
						PieceMoveSound.Play();
						spawnPieces["left"].Move(0, 1);
						spawnPieces["right"].Move(0, 1);
					}
					else if (event.key.keysym.sym == SDLK_z)
					{
						PieceMoveSound.Play();
						spawnPieces["right"].Rotate(-90.0f, spawnPieces["left"].GetPosition());
					}
					else if (event.key.keysym.sym == SDLK_x)
					{
						PieceMoveSound.Play();
						spawnPieces["right"].Rotate(90.0f, spawnPieces["left"].GetPosition());
					}
					else if (event.key.keysym.sym == SDLK_SPACE) // NOTE: Tests ONLY
					{
						SpawnNewPair();
					}
				}
			}

			// Clear screen
			renderer.Clear();

			// Draw
			// Draw Grid
			SDL_SetRenderDrawColor(renderer.GetSDLRenderer(), 255, 255, 255, SDL_ALPHA_OPAQUE);
			SDL_RenderDrawLine(renderer.GetSDLRenderer(), gridPositionX, gridPositionY, gridPositionX, (gridPositionY + gridHeight)); // Left Line
			SDL_RenderDrawLine(renderer.GetSDLRenderer(), gridPositionX, gridPositionY, (gridPositionX + gridWidth), gridPositionY); // Top Line
			SDL_RenderDrawLine(renderer.GetSDLRenderer(), (gridPositionX + gridWidth), gridPositionY, (gridPositionX + gridWidth), (gridPositionY + gridHeight)); // Right Line
			SDL_RenderDrawLine(renderer.GetSDLRenderer(), gridPositionX, (gridPositionY + gridHeight), (gridPositionX + gridWidth), (gridPositionY + gridHeight)); // Bottom Line

			// Draw score
			SDL_RenderDrawLine(renderer.GetSDLRenderer(), 10, gridPositionY, (10 + 250), gridPositionY); // Left Line
			SDL_RenderDrawLine(renderer.GetSDLRenderer(), 10, gridPositionY, 10, (gridPositionY + 100)); // Top Line
			SDL_RenderDrawLine(renderer.GetSDLRenderer(), (10 + 250), gridPositionY, (10 + 250), (gridPositionY + 100)); // Right Line
			SDL_RenderDrawLine(renderer.GetSDLRenderer(), 10, (gridPositionY + 100), (10 + 250), (gridPositionY + 100)); // Bottom Line
			renderer.DrawText(scoreText, { 25, gridPositionY + 20 });

			// Draw pieces
			for (auto& piece : pieces)
				renderer.DrawPiece(piece);

			// See if the time that has passed is equal to MAX_FRAMERATE(1s) times the amount of seconds to wait to move
			if (timeInGame == (MAX_FRAMERATE * 1))
			{
				for (auto& piece : spawnPieces)
					piece.second.Move(0, 1);

				timeInGame = 0;
			}
			for (auto& piece : spawnPieces)
				renderer.DrawPiece(piece.second);

			// Present
			renderer.Update();
		}
	}
	
	//Destroy window
	SDL_DestroyWindow(window);

	//Quit SDL subsystems
	SDL_Quit();

    return 0;
}