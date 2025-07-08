#pragma once
#include <SDL2/SDL.h>

typedef struct
{
	SDL_Window* window;
	SDL_Renderer* renderer;

	SDL_Event event;
	bool shouldQuit;

	bool inited;
} Graphics;

bool initGraphics(Graphics*, int w, int h);

void updateGraphics(Graphics*);

void destroyGraphics(Graphics*);
