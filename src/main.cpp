#include <iostream>
#include <SDL.h>

// Game Requirements
// - The game is played on a 8x16 grid -
// - Objective of the game is to match same colored pieces
// - There should be 4 different colored pieces
// - Forming groups of 4 + pieces, in L, T, square or other fully connecting shapes destroys the pieces
// - Pieces always appear in pairs, each piece may randomly share or differ in color to the other piece in the pair
// - Pairs are spawned above the top of the grid, dropping down until they are placed
// - The pair is considered placed when any of the pieces of the pair cannot be moved further down
// - The game is lost if it is not possible to place the pair within the grid
// - The player controls the pair’s movement by :
// - Moving the pair sideways
// - Rotating the pair in 90 degree angles(left or right)
// - Making the pair fall faster
// - Once the pair is placed
// - The player can no longer move the pair
// - The pieces will unpair and each of them will fall to the lowest position it can reach
// - Once there is no movement(all pieces placed), matches are validatedand removed from the grid
// - The next pair will be spawned once all matches are cleared
// - Check the following link for reference: youtube.com/watch?v=YJjRJ_4gcUw

//Screen dimension constants
constexpr int32_t SCREEN_WIDTH = 960;
constexpr int32_t SCREEN_HEIGHT = 720;

// Pieces dimensions
constexpr int32_t PIECE_SIZE = 32;

// Grid dimensions
constexpr int32_t GRID_WIDTH   =  8 * PIECE_SIZE;
constexpr int32_t GRID_HEIGHT  = 16 * PIECE_SIZE;
constexpr int32_t GRID_X       = (SCREEN_WIDTH  / 2) - (GRID_WIDTH  / 2);
constexpr int32_t GRID_Y	   = (SCREEN_HEIGHT / 2) - (GRID_HEIGHT / 2);

static bool s_IsRunning = true;

int main(int argc, char* args[])
{
	//The window we'll be rendering to
	SDL_Window* window = NULL;

	//The surface contained by the window
	SDL_Surface* screenSurface = NULL;

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

	auto renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (!renderer)
		printf("Renderer could not be initialized: %s\n", SDL_GetError());

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

			}
		}

		// Clear screen
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		// Draw
		// Draw Grid
		SDL_SetRenderDrawColor(renderer, 100, 100, 100, SDL_ALPHA_OPAQUE);
		SDL_RenderDrawLine(renderer, GRID_X, GRID_Y, GRID_X, (GRID_Y + GRID_HEIGHT));
		SDL_RenderDrawLine(renderer, GRID_X, GRID_Y, (GRID_X + GRID_WIDTH), GRID_Y);
		SDL_RenderDrawLine(renderer, (GRID_X + GRID_WIDTH), GRID_Y, (GRID_X + GRID_WIDTH), (GRID_Y + GRID_HEIGHT));
		SDL_RenderDrawLine(renderer, GRID_X, (GRID_Y + GRID_HEIGHT), (GRID_X + GRID_WIDTH), (GRID_Y + GRID_HEIGHT));

		// Draw score lines
		SDL_RenderDrawLine(renderer, 10, GRID_Y, (10 + 250), GRID_Y);
		SDL_RenderDrawLine(renderer, 10, GRID_Y, 10, (GRID_Y + 100));
		SDL_RenderDrawLine(renderer, (10 + 250), GRID_Y, (10 + 250), (GRID_Y + 100));
		SDL_RenderDrawLine(renderer, 10, (GRID_Y + 100), (10 + 250), (GRID_Y + 100));

		// Present
		SDL_RenderPresent(renderer);
	}
	
	//Destroy window
	SDL_DestroyWindow(window);

	//Quit SDL subsystems
	SDL_Quit();

    return 0;
}