#include <iostream>

#include <SDL.h>
#include <SDL_mixer.h>

#include <random>
#include <array>

#include "Renderer/Renderer.h"
#include "Piece.h"
#include "Sound/Music.h"
#include "Sound/SoundEffect.h"

// Game Requirements
// ✅ - The game is played on a 8x16 grid 
// - Objective of the game is to match same colored pieces
// ✅ - There should be 4 different colored pieces 
// - Forming groups of 4 + pieces, in L, T, square or other fully connecting shapes destroys the pieces
// ✅ - Pieces always appear in pairs, each piece may randomly share or differ in color to the other piece in the pair 
// ✅ - Pairs are spawned above the top of the grid, dropping down until they are placed 
// ✅ - The pair is considered placed when any of the pieces of the pair cannot be moved further down 
// ✅ - The game is lost if it is not possible to place the pair within the grid 
// ✅ - The player controls the pair’s movement by :
//		✅ - Moving the pair sideways 
//		✅ - Rotating the pair in 90 degree angles(left or right) 
//		✅ - Making the pair fall faster 
// - Once the pair is placed:
//		✅ - The player can no longer move the pair 
//		✅ - The pieces will unpair and each of them will fall to the lowest position it can reach 
//		- Once there is no movement(all pieces placed), matches are validated and removed from the grid
// - The next pair will be spawned once all matches are cleared
// ✅ - Rotacao para fora da grid
// ✅ - Check the following link for reference: youtube.com/watch?v=YJjRJ_4gcUw
// 
// Pluses
// - Spawnar tanto na horizontal como na vertical

#define MAX_FRAMERATE 60

//Screen dimension constants
constexpr uint32_t SCREEN_WIDTH = 960;
constexpr uint32_t SCREEN_HEIGHT = 720;

static bool s_IsRunning		= true;
static bool s_MusicEnabled	= false;
static bool s_IsGamePaused  = false;
static bool s_IsGameLost	= false;

Renderer renderer;

std::array<bool, 8> occupiedCells;

std::vector<std::shared_ptr<Piece>> lockedPieces;
std::unordered_map<const char*, std::shared_ptr<Piece>> spawnPieces;

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

/**
 * @returns true if the game is lost and return false if the game is not lost
 */
void CheckGameLost()
{
	// Update occupied cells array before checking if they are all true
	for (const auto& piece : lockedPieces)
	{
		// Check if any of the pieces is at the top row of the grid
		if (piece->GetPosition().second == (gridPositionY + PIECE_SIZE))
		{
			// if it is get the cell number of the piece and make it occupied on the array
			auto rowCellNumber = (piece->GetPosition().first - gridPositionX) / PIECE_SIZE;
			occupiedCells[rowCellNumber] = true;
		}
	}

	// See if every cell of the second row counting from the top is occupied
	if (std::all_of(occupiedCells.begin(), occupiedCells.end(), [](bool value) {return value == true; }))
		s_IsGameLost = true;
	else
		s_IsGameLost = false;
}

void SpawnNewPair()
{
	// Check if the game is lost before spawning a new pair
	CheckGameLost();

	if (!s_IsGameLost)
	{
		// Reset move timer
		timeInGame = 0;

		// Generate random number from 0 to 7 which is the number of cells horizontally on the grid,
		// then get the x position to spawn the piece by multiplying the random number that we got by the PIECE_SIZE 
		// and adding that up to the gridPositionX
		std::uniform_int_distribution<uint32_t> dist(0, 7);
		uint32_t cell = 0;

		// Get the cell number that is not occupied
		while (true)
		{
			cell = dist(mt);

			if (!occupiedCells[cell])
				break;
		}

		dist = std::uniform_int_distribution<uint32_t>(1, 4);
		auto color = dist(mt);

		uint32_t positionX = gridPositionX + (cell * PIECE_SIZE);

		// top piece
		std::shared_ptr<Piece> topPiece = std::make_shared<Piece>(Utils::IntToPieceColor(color), positionX, gridPositionY - (PIECE_SIZE * 2));
		topPiece->SetTexture(textureCache[Utils::PieceColorToString(topPiece->GetColor())]);
		spawnPieces["top"] = topPiece;

		// bottom piece
		color = dist(mt);
		std::shared_ptr<Piece> bottomPiece = std::make_shared<Piece>(Utils::IntToPieceColor(color), positionX, gridPositionY - PIECE_SIZE);
		bottomPiece->SetTexture(textureCache[Utils::PieceColorToString(bottomPiece->GetColor())]);
		spawnPieces["bottom"] = bottomPiece;
	}
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
	Text gameOverText(renderer, "Game Over!", teletoonInGame);

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
			// NOTE(Daniel): Dont run this while the game is over or if the game is paused
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
							if (!spawnPieces["bottom"]->IsLocked() && !spawnPieces["top"]->IsLocked())
							{
								if (spawnPieces["bottom"]->IsCollidingHoriontally(gridPositionX, gridWidth) != -1 &&
									spawnPieces["top"]->IsCollidingHoriontally(gridPositionX, gridWidth) != -1)
								{
									spawnPieces["bottom"]->Move(-1, 0);
									spawnPieces["top"]->Move(-1, 0);
								}
							}
						}
						else if (event.key.keysym.sym == SDLK_d || event.key.keysym.sym == SDLK_RIGHT)
						{
							if (!spawnPieces["bottom"]->IsLocked() && !spawnPieces["top"]->IsLocked())
							{
								if (spawnPieces["bottom"]->IsCollidingHoriontally(gridPositionX, gridWidth) != 1 &&
									spawnPieces["top"]->IsCollidingHoriontally(gridPositionX, gridWidth) != 1)
								{
									spawnPieces["bottom"]->Move(1, 0);
									spawnPieces["top"]->Move(1, 0);
								}
							}
						}
						else if (event.key.keysym.sym == SDLK_s || event.key.keysym.sym == SDLK_DOWN)
						{
							if (!spawnPieces["bottom"]->IsLocked() && !spawnPieces["top"]->IsLocked())
							{
								spawnPieces["bottom"]->Move(0, 1);
								spawnPieces["top"]->Move(0, 1);
								timeInGame = 0;
							}
						}
						else if (event.key.keysym.sym == SDLK_z)
						{
							// TODO: Optimize this code
							if (spawnPieces["top"]->IsCollidingHoriontally(gridPositionX, gridWidth) == -1)
							{
								if (spawnPieces["top"]->GetRotation() != 0)
								{
									if (!spawnPieces["bottom"]->IsLocked() && !spawnPieces["top"]->IsLocked())
										spawnPieces["top"]->Rotate(-90.0f, spawnPieces["bottom"]->GetPosition());
								}
							}
							else if (spawnPieces["top"]->IsCollidingHoriontally(gridPositionX, gridWidth) == 1)
							{
								if (spawnPieces["top"]->GetRotation() == 0)
								{
									if (!spawnPieces["bottom"]->IsLocked() && !spawnPieces["top"]->IsLocked())
										spawnPieces["top"]->Rotate(-90.0f, spawnPieces["bottom"]->GetPosition());
								}
								else if (spawnPieces["top"]->GetRotation() % 180 != 0)
								{
									if (!spawnPieces["bottom"]->IsLocked() && !spawnPieces["top"]->IsLocked())
										spawnPieces["top"]->Rotate(-90.0f, spawnPieces["bottom"]->GetPosition());
								}
							}
							else
							{
								if (!spawnPieces["bottom"]->IsLocked() && !spawnPieces["top"]->IsLocked())
									spawnPieces["top"]->Rotate(-90.0f, spawnPieces["bottom"]->GetPosition());
							}
							
						}
						else if (event.key.keysym.sym == SDLK_x)
						{
							if (spawnPieces["top"]->IsCollidingHoriontally(gridPositionX, gridWidth) == -1)
							{
								if (spawnPieces["top"]->GetRotation() == 0)
								{
									if (!spawnPieces["bottom"]->IsLocked() && !spawnPieces["top"]->IsLocked())
										spawnPieces["top"]->Rotate(90.0f, spawnPieces["bottom"]->GetPosition());
								}
								else if (spawnPieces["top"]->GetRotation() % 180 != 0)
								{
									if (!spawnPieces["bottom"]->IsLocked() && !spawnPieces["top"]->IsLocked())
										spawnPieces["top"]->Rotate(90.0f, spawnPieces["bottom"]->GetPosition());
								}
							}
							else if (spawnPieces["top"]->IsCollidingHoriontally(gridPositionX, gridWidth) == 1)
							{
								if (spawnPieces["top"]->GetRotation() != 0)
								{
									if (!spawnPieces["bottom"]->IsLocked() && !spawnPieces["top"]->IsLocked())
										spawnPieces["top"]->Rotate(90.0f, spawnPieces["bottom"]->GetPosition());
								}
							}
							else
							{
								if (!spawnPieces["bottom"]->IsLocked() && !spawnPieces["top"]->IsLocked())
									spawnPieces["top"]->Rotate(90.0f, spawnPieces["bottom"]->GetPosition());
							}
						}
					}

					if (!s_IsGameLost)
					{
						if (event.key.keysym.sym == SDLK_ESCAPE)
						{
							s_IsGamePaused = !s_IsGamePaused;
						}
					}
				}
			}

			// Clear screen
			renderer.Clear();

			if (!s_IsGameLost && !s_IsGamePaused)
			{
				// Check if any of the spawned pieces has reached the end of the grid
				if (spawnPieces["bottom"]->IsCollidingVertically(gridPositionY, gridHeight) ||
					spawnPieces["top"]->IsCollidingVertically(gridPositionY, gridHeight))
				{
					spawnPieces["bottom"]->SetLocked(true);
					spawnPieces["top"]->SetLocked(true);
				}

				// Check if any of the spawned pieces is colliding with any of the locked pieces on the grid
				for (const auto& piece : lockedPieces)
				{
					if (piece->IsCollidingWithPiece(*spawnPieces["top"]))
						spawnPieces["top"]->SetLocked(true);

					if (piece->IsCollidingWithPiece(*spawnPieces["bottom"]))
						spawnPieces["bottom"]->SetLocked(true);
				}

				// If both spawned pieces are locked, check the matches, 
				// put the spawned pieces inside the locked pieces vector and spawn new ones
				if (spawnPieces["top"]->IsLocked() && spawnPieces["bottom"]->IsLocked())
				{
					lockedPieces.emplace_back(spawnPieces["bottom"]);
					lockedPieces.emplace_back(spawnPieces["top"]);

					SpawnNewPair();
				}
				else if (spawnPieces["top"]->IsLocked() || spawnPieces["bottom"]->IsLocked())
				{
					if (timeInGame == (MAX_FRAMERATE / 10))
					{
						for (auto& piece : spawnPieces)
							piece.second->Move(0, 1);

						timeInGame = 0;
					}

					if (spawnPieces["bottom"]->IsCollidingWithPiece(*spawnPieces["top"]))
					{
						if (spawnPieces["bottom"]->GetRotation() % 180 == 0 ||
							spawnPieces["top"]->GetRotation() % 180 == 0)
						{
							spawnPieces["top"]->SetLocked(true);
							spawnPieces["bottom"]->SetLocked(true);
						}
					}
				}

				// Move spawned pieces every 1sec
				if (timeInGame == (MAX_FRAMERATE * 1))
				{
					for (auto& piece : spawnPieces)
						piece.second->Move(0, 1);

					timeInGame = 0;
				}
			}

			// Draw
			// Draw background under everything
			renderer.DrawRenderable(background);

			for (auto& piece : lockedPieces)
				renderer.DrawRenderable(*piece);

			for (auto& piece : spawnPieces)
			{
				// Only render the spawn pieces when they are inside the grid,
				// don't render them when they are above it
				//if (piece.second->GetPosition().second >= gridPositionY)
					renderer.DrawRenderable(*piece.second);
			}

			// NOTE: Render this at last so it stays on top of everything
			// Draw Grid
			SDL_SetRenderDrawColor(renderer.GetSDLRenderer(), 255, 255, 255, SDL_ALPHA_OPAQUE);
			SDL_RenderDrawLine(renderer.GetSDLRenderer(), gridPositionX, gridPositionY, gridPositionX, (gridPositionY + gridHeight)); // bottom Line
			SDL_RenderDrawLine(renderer.GetSDLRenderer(), gridPositionX, gridPositionY, (gridPositionX + gridWidth), gridPositionY); // Top Line
			SDL_RenderDrawLine(renderer.GetSDLRenderer(), (gridPositionX + gridWidth), gridPositionY, (gridPositionX + gridWidth), (gridPositionY + gridHeight)); // top Line
			SDL_RenderDrawLine(renderer.GetSDLRenderer(), gridPositionX, (gridPositionY + gridHeight), (gridPositionX + gridWidth), (gridPositionY + gridHeight)); // Bottom Line

			// Draw score
			SDL_RenderDrawLine(renderer.GetSDLRenderer(), 10, gridPositionY, (10 + 250), gridPositionY); // bottom Line
			SDL_RenderDrawLine(renderer.GetSDLRenderer(), 10, gridPositionY, 10, (gridPositionY + 100)); // Top Line
			SDL_RenderDrawLine(renderer.GetSDLRenderer(), (10 + 250), gridPositionY, (10 + 250), (gridPositionY + 100)); // top Line
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

			if (s_IsGameLost)
			{
				SDL_SetRenderDrawColor(renderer.GetSDLRenderer(), 0, 0, 0, 150);
				SDL_Rect rect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
				SDL_RenderFillRect(renderer.GetSDLRenderer(), &rect);
				renderer.DrawText(gameOverText, (SCREEN_WIDTH / 2) - (gameOverText.GetRectSize().first / 2), 250);
				GameOverMusic.Play(false);
			}

			// Present
			renderer.Update();
		}
	}
	
	backgroundMusic.Destroy();
	GameOverMusic.Destroy();
	PieceDropSound.Destroy();
	PieceGroupRemoveSound.Destroy();

	for (const auto& texture : textureCache)
		SDL_DestroyTexture(texture.second);
	textureCache.clear();

	//Destroy window
	SDL_DestroyWindow(window);

	// Close audio mixer
	Mix_CloseAudio();

	//Quit SDL subsystems
	SDL_Quit();

    return 0;
}