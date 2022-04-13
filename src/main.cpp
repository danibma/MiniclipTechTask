#include <iostream>

#include <SDL.h>
#include <SDL_mixer.h>

#include <random>

#include "Renderer/Renderer.h"
#include "Piece.h"
#include "Sound/Music.h"
#include "Sound/SoundEffect.h"

// Game Requirements
// - The game is played on a 8x16 grid ✅
// - Objective of the game is to match same colored pieces
// - There should be 4 different colored pieces ✅
// - Forming groups of 4 + pieces, in L, T, square or other fully connecting shapes destroys the pieces
// - Pieces always appear in pairs, each piece may randomly share or differ in color to the other piece in the pair ✅
// - Pairs are spawned above the top of the grid, dropping down until they are placed ✅
// - The pair is considered placed when any of the pieces of the pair cannot be moved further down
// - The game is lost if it is not possible to place the pair within the grid
// - The player controls the pair’s movement by :
//		- Moving the pair sideways ✅
//		- Rotating the pair in 90 degree angles(left or right) ✅
//		- Making the pair fall faster ✅
// - Once the pair is placed:
//		- The player can no longer move the pair
//		- The pieces will unpair and each of them will fall to the lowest position it can reach
//		- Once there is no movement(all pieces placed), matches are validated and removed from the grid
// - The next pair will be spawned once all matches are cleared
// - Check the following link for reference: youtube.com/watch?v=YJjRJ_4gcUw

#define MAX_FRAMERATE 60

//Screen dimension constants
constexpr uint32_t SCREEN_WIDTH = 960;
constexpr uint32_t SCREEN_HEIGHT = 720;

static bool s_IsRunning		= true;
static bool s_MusicEnabled	= false;
static bool s_IsGamePaused  = false;

Renderer renderer;

std::vector<Piece> pieces;
std::unordered_map<const char*, Piece> spawnPieces;

// pieces colors textures
std::unordered_map<const char*, SDL_Texture*> textureCache;

// Grid dimensions
int32_t gridWidth = 8 * PIECE_SIZE;
int32_t gridHeight = 16 * PIECE_SIZE;
int32_t gridPositionX = (SCREEN_WIDTH / 2) - (gridWidth / 2);
int32_t gridPositionY = (SCREEN_HEIGHT / 2) - (gridHeight / 2);

// Random number engine
std::random_device rd;
std::mt19937_64 mt(rd());

int timeInGame = 0;

void SpawnNewPair()
{
	// Generate random number from 0 to 6 which is the number of cells horizontally on the grid,
	// its 6 because the last cell would be the seventh, but we are spawning a pair of pieces, so the first piece has to spawn on
	// the sixth, then get the x position to spawn the piece, by multiplying the random number that we got by the PIECE_SIZE 
	// and adding that up to the gridPositionX

	timeInGame = 0;

	std::uniform_int_distribution<uint32_t> dist(0, 6);
	auto cell = dist(mt);

	dist = std::uniform_int_distribution<uint32_t>(1, 4);
	auto color = dist(mt);

	uint32_t positionX = gridPositionX + (cell * PIECE_SIZE);

	// Left piece
	Piece leftPiece(Utils::IntToPieceColor(color), positionX, gridPositionY - PIECE_SIZE);
	leftPiece.SetTexture(textureCache[Utils::PieceColorToString(leftPiece.GetColor())]);
	spawnPieces["left"] = leftPiece;

	// Right piece
	color = dist(mt);
	Piece rightPiece(Utils::IntToPieceColor(color), positionX + PIECE_SIZE, gridPositionY - PIECE_SIZE);
	rightPiece.SetTexture(textureCache[Utils::PieceColorToString(rightPiece.GetColor())]);
	spawnPieces["right"] = rightPiece;
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
	SoundEffect PieceDropSound("assets/sounds/sound_effects/piece_drop.mp3");
	SoundEffect PieceGroupRemoveSound("assets/sounds/sound_effects/piece_group_remove.mp3");

	if (s_MusicEnabled)
		backgroundMusic.Play(true);

	// Initialize Renderer
	renderer.Init(window, SCREEN_WIDTH, SCREEN_HEIGHT);
	
	Font teletoonInGame("assets/Fonts/Teletoon.ttf", 42);
	Text scoreText(renderer, "Score: 0", teletoonInGame);
	Text pausedText(renderer, "Game Paused!", teletoonInGame);
	Text removePauseText(renderer, "Press Escape again to unpause!", teletoonInGame);

	//Timer
	float lastFrame = 0;
	float time;
	float timestep;
	float maxPeriod = (float)1000 / MAX_FRAMERATE;

	// Load colors respective texture
	textureCache[Utils::PieceColorToString(PieceColor::Green)] = renderer.CreateTexture("assets/green.bmp");
	textureCache[Utils::PieceColorToString(PieceColor::LightBlue)] = renderer.CreateTexture("assets/lightblue.bmp");
	textureCache[Utils::PieceColorToString(PieceColor::Orange)] = renderer.CreateTexture("assets/orange.bmp");
	textureCache[Utils::PieceColorToString(PieceColor::Red)] = renderer.CreateTexture("assets/red.bmp");
	textureCache["background"] = renderer.CreateTexture("assets/background.bmp");

	// Spawn the first pair of pieces
	SpawnNewPair();

	Renderable background(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, textureCache["background"]);

	while (s_IsRunning)
	{
		time = (float)SDL_GetTicks();
		timestep = time - lastFrame;

		if (timestep >= maxPeriod)
		{
			// NOTE(Daniel): Dont run this while the game is over and the game is on main menu
			if (!s_IsGamePaused)
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
					if (!s_IsGamePaused)
					{
						if (event.key.keysym.sym == SDLK_a || event.key.keysym.sym == SDLK_LEFT)
						{
							if (spawnPieces["left"].IsCollidingHoriontally(gridPositionX, gridWidth) != -1 &&
								spawnPieces["right"].IsCollidingHoriontally(gridPositionX, gridWidth) != -1)
							{
								spawnPieces["left"].Move(-1, 0);
								spawnPieces["right"].Move(-1, 0);
							}
						}
						else if (event.key.keysym.sym == SDLK_d || event.key.keysym.sym == SDLK_RIGHT)
						{
							if (spawnPieces["left"].IsCollidingHoriontally(gridPositionX, gridWidth) != 1 &&
								spawnPieces["right"].IsCollidingHoriontally(gridPositionX, gridWidth) != 1)
							{
								spawnPieces["left"].Move(1, 0);
								spawnPieces["right"].Move(1, 0);
							}
						}
						else if (event.key.keysym.sym == SDLK_s || event.key.keysym.sym == SDLK_DOWN)
						{
							spawnPieces["left"].Move(0, 1);
							spawnPieces["right"].Move(0, 1);
						}
						else if (event.key.keysym.sym == SDLK_z)
						{
							spawnPieces["right"].Rotate(-90.0f, spawnPieces["left"].GetPosition());
						}
						else if (event.key.keysym.sym == SDLK_x)
						{
							spawnPieces["right"].Rotate(90.0f, spawnPieces["left"].GetPosition());
						}
						else if (event.key.keysym.sym == SDLK_SPACE) // NOTE: Tests ONLY
						{
							SpawnNewPair();
						}
					}

					if (event.key.keysym.sym == SDLK_ESCAPE)
					{
						s_IsGamePaused = !s_IsGamePaused;
					}
				}
			}

			// Clear screen
			renderer.Clear();

			// Check if any of the spawned pieces has reached the end of the grid
			if (spawnPieces["left"].IsCollidingVertically(gridPositionY, gridHeight))
				spawnPieces["left"].SetLocked(true);

			if (spawnPieces["right"].IsCollidingVertically(gridPositionY, gridHeight))
				spawnPieces["right"].SetLocked(true);


			// Move spawned pieces every 1sec
			if (timeInGame == (MAX_FRAMERATE * 1))
			{
				for (auto& piece : spawnPieces)
					piece.second.Move(0, 1);

				timeInGame = 0;
			}

			// Draw
			// Draw background under everything
			renderer.DrawRenderable(background);

			for (auto& piece : pieces)
				renderer.DrawRenderable(piece);

			for (auto& piece : spawnPieces)
			{
				// Only render the spawn pieces when they are inside the grid,
				// don't render them when they are above it
				if (piece.second.GetPosition().second >= gridPositionY)
					renderer.DrawRenderable(piece.second);
			}

			// NOTE: Render this at last so it stays on top of everything
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

			// Draw Menu's
			if (s_IsGamePaused)
			{
				SDL_SetRenderDrawColor(renderer.GetSDLRenderer(), 0, 0, 0, 150);
				SDL_Rect rect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
				SDL_RenderFillRect(renderer.GetSDLRenderer(), &rect);
				renderer.DrawText(pausedText, (SCREEN_WIDTH / 2) - (pausedText.GetRectSize().first / 2), 250);
				renderer.DrawText(removePauseText, (SCREEN_WIDTH / 2) - (removePauseText.GetRectSize().first / 2), 300);
			}

			// Present
			renderer.Update();
		}
	}
	
	for (const auto& texture : textureCache)
		SDL_DestroyTexture(texture.second);
	textureCache.clear();

	//Destroy window
	SDL_DestroyWindow(window);

	//Quit SDL subsystems
	SDL_Quit();

    return 0;
}