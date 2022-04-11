#include <iostream>
#include <SDL.h>

//Screen dimension constants
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

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

	auto renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	//Create window
	window = SDL_CreateWindow("Miniclip Tech Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_VULKAN);
	if (!window)
	{
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		return 0;
	}

	while (s_IsRunning)
	{

		SDL_Event event;
		if (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
				s_IsRunning = false;

			if (event.type == SDL_MOUSEBUTTONDOWN)
				printf("TODO: mouse click!\n");
		}

		//Get window surface
		screenSurface = SDL_GetWindowSurface(window);

		//Fill the surface white
		SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xAF, 0xFF, 0xFF));

		//Update the surface
		SDL_UpdateWindowSurface(window);

		SDL_RenderPresent(renderer);
	}
	
	//Destroy window
	SDL_DestroyWindow(window);

	//Quit SDL subsystems
	SDL_Quit();

    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
