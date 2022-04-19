#include <iostream>

#include <SDL.h>
#include <SDL_mixer.h>

#include <random>
#include <array>

#include "Renderer/Renderer.h"
#include "Piece.h"
#include "Sound/Music.h"
#include "Sound/SoundEffect.h"
#include "UI/Button.h"

#define MAX_FRAMERATE 60

//Screen dimension constants
constexpr uint32_t SCREEN_WIDTH = 960;
constexpr uint32_t SCREEN_HEIGHT = 720;

enum class GameState
{
	kGameRunning,
	kGamePaused,
	kGameLost,
	kGameMainMenu
} s_GameState;

static bool s_IsRunning		= true;
static bool s_MusicEnabled	= false;

Renderer renderer;

std::array<bool, 8> occupiedCells;

std::vector<std::shared_ptr<Piece>> combinedPieces;
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
		s_GameState = GameState::kGameLost;
}

void SpawnNewPair()
{
	// Check if the game is lost before spawning a new pair
	CheckGameLost();

	if (s_GameState == GameState::kGameRunning)
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

uint32_t GetCombinedPieces(std::shared_ptr<Piece> piece)
{
	auto [centerX, centerY] = piece->GetPosition();

	uint32_t combinedPiecesCount = 0;

	for (int i = 0; i <= 3; i++)
	{
		// start out checking the top piece
		int32_t nextPieceX = centerX;
		int32_t nextPieceY = centerY - PIECE_SIZE;

		double angleInRadians = ToRadians(90 * i);

		double x = cos(angleInRadians) * (nextPieceX - centerX) - sin(angleInRadians) * (nextPieceY - centerY) + centerX;
		double y = sin(angleInRadians) * (nextPieceX - centerX) + cos(angleInRadians) * (nextPieceY - centerY) + centerY;

		nextPieceX = (int32_t)x;
		nextPieceY = (int32_t)y;

		auto nextPiece = std::find_if(lockedPieces.begin(), lockedPieces.end(), [&](std::shared_ptr<Piece> currentPiece) {
			auto [currentX, currentY] = currentPiece->GetPosition();

			return currentX == nextPieceX && currentY == nextPieceY;
		});

		if (nextPiece != lockedPieces.end() && nextPiece[0] != piece)
		{
			if (std::find(combinedPieces.begin(), combinedPieces.end(), nextPiece[0]) != combinedPieces.end())
				continue;

			if (nextPiece[0]->GetColor() == piece->GetColor())
			{
				combinedPieces.emplace_back(nextPiece[0]);
				combinedPiecesCount++;
				combinedPiecesCount += GetCombinedPieces(nextPiece[0]);
			}
		}
	}

	return combinedPiecesCount;
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
	
	Font teletoonInGame("assets/Fonts/Teletoon.ttf", 37);
	Text pausedText(renderer, "Game Paused!", teletoonInGame);
	Text removePauseText(renderer, "Press Escape again to unpause!", teletoonInGame);
	Text gameOverText(renderer, "Game Over!", teletoonInGame);

	// Score
	uint32_t score = 0;
	char scoreBuffer[16] = "Score: 0";
	Text scoreText(renderer, scoreBuffer, teletoonInGame);

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
	textureCache["buttonBackground"] = renderer.CreateTexture("assets/button.bmp");
	textureCache["buttonPressed"] = renderer.CreateTexture("assets/button_pressed.bmp");

	Renderable background(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, textureCache["background"]);

	// Main Menu
	Text playButtonText(renderer, "Play Game", teletoonInGame);
	Button playButton(playButtonText, textureCache["buttonBackground"], textureCache["buttonPressed"], { (SCREEN_WIDTH / 2) - (350 / 2), 150}, {350, 80});
	playButton.SetOnClickCallback([]() {
		s_GameState = GameState::kGameRunning;

		// Spawn the first pair of pieces
		SpawnNewPair();
	});

	Text quitButtonText(renderer, "Quit Game", teletoonInGame);
	Button quitButton(quitButtonText, textureCache["buttonBackground"], textureCache["buttonPressed"], { (SCREEN_WIDTH / 2) - (350 / 2), 250 }, { 350, 80 });
	quitButton.SetOnClickCallback([]() {
		s_IsRunning = false;
	});

	Font teletoonControlsTitle("assets/Fonts/Teletoon.ttf", 32);
	Text howToPlayText(renderer, "How To Play:", teletoonControlsTitle);

	std::vector<Text> controlsTexts;
	controlsTexts.reserve(6);
	Font teletoonControls("assets/Fonts/Teletoon.ttf", 25);
	controlsTexts.emplace_back(renderer, "Move Left: A or Left Arrow", teletoonControls);
	controlsTexts.emplace_back(renderer, "Move Right: D or Right Arrow", teletoonControls);
	controlsTexts.emplace_back(renderer, "Move Down: S or Down Arrow", teletoonControls);
	controlsTexts.emplace_back(renderer, "Rotate Left: Z", teletoonControls);
	controlsTexts.emplace_back(renderer, "Rotate Right: X", teletoonControls);
	controlsTexts.emplace_back(renderer, "Pause/Unpause: Escape", teletoonControls);

	// Start on the main menu
	s_GameState = GameState::kGameMainMenu;

	while (s_IsRunning)
	{
		time = (float)SDL_GetTicks();
		timestep = time - lastFrame;

		if (timestep >= maxPeriod)
		{
			lastFrame = time;

			SDL_Event event;
			if (SDL_PollEvent(&event))
			{
				if (event.type == SDL_QUIT)
					s_IsRunning = false;

				if (s_GameState == GameState::kGameMainMenu)
				{
					if (event.type == SDL_MOUSEBUTTONDOWN)
					{
						if (playButton.IsMouseOver())
							playButton.OnPressed();

						if (quitButton.IsMouseOver())
							quitButton.OnPressed();
					}

					if (event.type == SDL_MOUSEBUTTONUP)
					{
						playButton.OnRelease();
						quitButton.OnRelease();
					}
				}

				if (event.type == SDL_KEYDOWN)
				{
					// Keybinds
					if (s_GameState == GameState::kGameRunning)
					{
						if (event.key.keysym.sym == SDLK_a || event.key.keysym.sym == SDLK_LEFT)
						{
							if (!spawnPieces["bottom"]->IsLocked() && !spawnPieces["top"]->IsLocked())
							{
								if (spawnPieces["bottom"]->IsCollidingHoriontally(gridPositionX, gridWidth) != -1 &&
									spawnPieces["top"]->IsCollidingHoriontally(gridPositionX, gridWidth) != -1)
								{
									// Check if the spawn pieces are colliding with any of the locked pieces
									bool canMove = true;

									for (const auto& piece : lockedPieces)
									{
										if (spawnPieces["top"]->IsCollidingWithPieceHorizontally(*piece) == -1)
											canMove = false;

										if (spawnPieces["bottom"]->IsCollidingWithPieceHorizontally(*piece) == -1)
											canMove = false;
									}

									if (canMove)
									{
										spawnPieces["bottom"]->Move(-1, 0);
										spawnPieces["top"]->Move(-1, 0);
									}
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
									// Check if the spawn pieces are colliding with any of the locked pieces
									bool canMove = true;

									for (const auto& piece : lockedPieces)
									{
										if (spawnPieces["top"]->IsCollidingWithPieceHorizontally(*piece) == 1)
											canMove = false;

										if (spawnPieces["bottom"]->IsCollidingWithPieceHorizontally(*piece) == 1)
											canMove = false;
									}

									if (canMove)
									{
										spawnPieces["bottom"]->Move(1, 0);
										spawnPieces["top"]->Move(1, 0);
									}
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
							bool canRotate = false;

							// Check if the piece has space to rotate without getting out of the grid
							if (spawnPieces["top"]->IsCollidingHoriontally(gridPositionX, gridWidth) == -1)
							{
								if (spawnPieces["top"]->GetRotation() != PieceRotation::Top)
									canRotate = true;
							}
							else if (spawnPieces["top"]->IsCollidingHoriontally(gridPositionX, gridWidth) == 1)
							{
								if (spawnPieces["top"]->GetRotation() == PieceRotation::Top)
									canRotate = true;
								else if (spawnPieces["top"]->GetRotation() != PieceRotation::Down)
									canRotate = true;
							}
							else
							{
								canRotate = true;
							}

							// Check if the piece has space to rotate and won't collide with any locked piece
							for (const auto& piece : lockedPieces)
							{
								if (spawnPieces["bottom"]->IsCollidingWithPieceHorizontally(*piece) == -1)
								{
									if (spawnPieces["top"]->GetRotation() == PieceRotation::Top)
										canRotate = false;

								}
								else if (spawnPieces["bottom"]->IsCollidingWithPieceHorizontally(*piece) == 1)
								{
									if (spawnPieces["top"]->GetRotation() == PieceRotation::Down)
										canRotate = false;
								}
							}

							if (canRotate)
							{
								if (!spawnPieces["bottom"]->IsLocked() && !spawnPieces["top"]->IsLocked())
									spawnPieces["top"]->Rotate(-90, spawnPieces["bottom"]->GetPosition());
							}
							
						}
						else if (event.key.keysym.sym == SDLK_x)
						{
							bool canRotate = false;

							// Check if the piece has space to rotate without getting out of the grid
							if (spawnPieces["top"]->IsCollidingHoriontally(gridPositionX, gridWidth) == -1)
							{
								if (spawnPieces["top"]->GetRotation() == PieceRotation::Top)
									canRotate = true;
								else if (spawnPieces["top"]->GetRotation() != PieceRotation::Down)
									canRotate = true;
							}
							else if (spawnPieces["top"]->IsCollidingHoriontally(gridPositionX, gridWidth) == 1)
							{
								if (spawnPieces["top"]->GetRotation() != PieceRotation::Top)
									canRotate = true;
							}
							else
							{
								canRotate = true;
							}

							// Check if the piece has space to rotate and won't collide with any locked piece
							for (const auto& piece : lockedPieces)
							{
								if (spawnPieces["bottom"]->IsCollidingWithPieceHorizontally(*piece) == -1)
								{
									if (spawnPieces["top"]->GetRotation() == PieceRotation::Down)
										canRotate = false;
									
								}
								else if (spawnPieces["bottom"]->IsCollidingWithPieceHorizontally(*piece) == 1)
								{
									if (spawnPieces["top"]->GetRotation() == PieceRotation::Top)
										canRotate = false;
								}
							}

							if (canRotate)
							{
								if (!spawnPieces["bottom"]->IsLocked() && !spawnPieces["top"]->IsLocked())
									spawnPieces["top"]->Rotate(90, spawnPieces["bottom"]->GetPosition());
							}
						}
					}

					if (s_GameState != GameState::kGameLost && s_GameState != GameState::kGameMainMenu)
					{
						if (event.key.keysym.sym == SDLK_ESCAPE)
						{
							if(s_GameState == GameState::kGamePaused)
								s_GameState = GameState::kGameRunning;
							else
								s_GameState = GameState::kGamePaused;
						}
					}
				}
			}

			// Clear screen
			renderer.Clear();

			if (s_GameState == GameState::kGameRunning)
			{
				timeInGame++;

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
					if (piece->IsCollidingWithPieceVertically(*spawnPieces["top"]))
						spawnPieces["top"]->SetLocked(true);

					if (piece->IsCollidingWithPieceVertically(*spawnPieces["bottom"]))
						spawnPieces["bottom"]->SetLocked(true);
				}

				// If both spawned pieces are locked, check the matches, 
				// put the spawned pieces inside the locked pieces vector and spawn new ones
				if (spawnPieces["top"]->IsLocked() && spawnPieces["bottom"]->IsLocked())
				{
					lockedPieces.emplace_back(spawnPieces["bottom"]);
					lockedPieces.emplace_back(spawnPieces["top"]);
					PieceDropSound.Play();

					// Check for matches
					for (const auto& piece : lockedPieces)
					{
						uint32_t combinedPiecesCount = GetCombinedPieces(piece);

						if (combinedPiecesCount >= 4)
						{
							for (auto& combinedPiece : combinedPieces)
							{
								lockedPieces.erase(std::remove(lockedPieces.begin(), lockedPieces.end(), combinedPiece), lockedPieces.end());

								// Unlock pieces so they can move again until they collide with another piece or with the grid
								for (auto& pieceToMove : lockedPieces)
								{
									if (pieceToMove->GetPosition().second <= combinedPiece->GetPosition().second &&
										pieceToMove->GetPosition().first == combinedPiece->GetPosition().first)
									{
										pieceToMove->SetLocked(false);
									}
								}
							}
							
							score += 5 * combinedPiecesCount;
							scoreText.UpdateText(renderer, "Score: " + std::to_string(score));
							
							// TODO: destroy multiple combinations of pieces, right now is just destroying one combination
 							break;
						}

						combinedPieces.clear();
					}

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

					if (spawnPieces["bottom"]->IsCollidingWithPieceVertically(*spawnPieces["top"]))
					{
						if (spawnPieces["top"]->GetRotation() == PieceRotation::Down ||
							spawnPieces["top"]->GetRotation() == PieceRotation::Top)
						{
							spawnPieces["top"]->SetLocked(true);
							spawnPieces["bottom"]->SetLocked(true);
						}
					}
				}

				// Move the locked pieces when they get unlocked
				// They get unlocked when a set of combined pieces is destroyed
				if (!std::all_of(lockedPieces.begin(), lockedPieces.end(), [](std::shared_ptr<Piece> piece){ return piece->IsLocked(); }))
				{
					if (lockedPieces.size() == 1)
					{
						if (lockedPieces[0]->IsCollidingVertically(gridPositionY, gridHeight))
							lockedPieces[0]->SetLocked(true);

						lockedPieces[0]->Move(0, 1);
					}
					else
					{
						for (auto& pieceToMove : lockedPieces)
						{
							for (auto& piece : lockedPieces)
							{
								if (piece == pieceToMove)
									continue;

								if (piece->IsLocked())
								{
									if (pieceToMove->IsCollidingVertically(gridPositionY, gridHeight) ||
										pieceToMove->IsCollidingWithPieceVertically(*piece))
									{
										pieceToMove->SetLocked(true);
									}
								}
								else
								{
									if (pieceToMove->IsCollidingVertically(gridPositionY, gridHeight))
										pieceToMove->SetLocked(true);
								}
							}

							pieceToMove->Move(0, 1);
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
				if (piece.second->GetPosition().second >= gridPositionY)
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
			renderer.DrawText(scoreText, 25, gridPositionY + 20);

			// Draw Menu's
			switch (s_GameState)
			{
			SDL_Rect rect;
			case GameState::kGamePaused:
				SDL_SetRenderDrawColor(renderer.GetSDLRenderer(), 0, 0, 0, 150);
				 rect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
				SDL_RenderFillRect(renderer.GetSDLRenderer(), &rect);
				renderer.DrawText(pausedText, (SCREEN_WIDTH / 2) - (pausedText.GetRectSize().first / 2), 250);
				renderer.DrawText(removePauseText, (SCREEN_WIDTH / 2) - (removePauseText.GetRectSize().first / 2), 300);
				break;
			case GameState::kGameLost:
				SDL_SetRenderDrawColor(renderer.GetSDLRenderer(), 0, 0, 0, 150);
				rect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
				SDL_RenderFillRect(renderer.GetSDLRenderer(), &rect);
				renderer.DrawText(gameOverText, (SCREEN_WIDTH / 2) - (gameOverText.GetRectSize().first / 2), 250);
				GameOverMusic.Play(false);
				break;
			case GameState::kGameMainMenu:
				SDL_SetRenderDrawColor(renderer.GetSDLRenderer(), 0, 0, 0, 220);
				rect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
				SDL_RenderFillRect(renderer.GetSDLRenderer(), &rect);
				renderer.DrawButton(playButton);
				renderer.DrawButton(quitButton);
				renderer.DrawText(howToPlayText, (SCREEN_WIDTH / 2) - (howToPlayText.GetRectSize().first / 2), 350);
				for (int i = 0; i < controlsTexts.size(); ++i)
				{
					renderer.DrawText(controlsTexts[i], (SCREEN_WIDTH / 2) - (controlsTexts[i].GetRectSize().first / 2), 400 + (30 * i));
				}
				break;
			}

			// Present
			renderer.Update();
		}
	}
	
	backgroundMusic.Destroy();
	GameOverMusic.Destroy();
	PieceDropSound.Destroy();
	PieceGroupRemoveSound.Destroy();

	teletoonInGame.CloseFont();
	teletoonControlsTitle.CloseFont();
	teletoonControls.CloseFont();

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