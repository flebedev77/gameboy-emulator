#include "graphics.h"

bool initGraphics(Graphics* g, int width, int height)
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL: could not initialise: %s\n", SDL_GetError());
		return 1;
	}	

	g->window = SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);

	if (g->window == NULL)
	{
		printf("SDL: window could not be created: %s\n", SDL_GetError());
		SDL_Quit();
		return 1;
	}
	
	g->renderer = SDL_CreateRenderer(g->window, -1, SDL_RENDERER_ACCELERATED);
	if (g->renderer == NULL)
	{
		printf("SDL: render could not be created: %s\n", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	g->inited = true;

	g->shouldQuit = false;

	return 0;
}

void updateGraphics(Graphics* g)
{
	bool pollStatus = SDL_PollEvent(&g->event);

	if (pollStatus != 0)
	{
		if (g->event.type == SDL_QUIT)
		{
			g->shouldQuit = true;
		}
	}


	SDL_SetRenderDrawColor(g->renderer, 0, 0, 0, 255);
	SDL_RenderClear(g->renderer);
	SDL_RenderPresent(g->renderer);
}

void destroyGraphics(Graphics* g)
{
	SDL_DestroyRenderer(g->renderer);
	SDL_DestroyWindow(g->window);
	SDL_Quit();
}
