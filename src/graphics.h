#pragma once
#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdio.h>
#include "ppudecode.h"
#include "util.h"
#include "globals.h"

typedef struct
{
	SDL_Window* window;
	SDL_Renderer* renderer;

	SDL_Event event;
	bool shouldQuit;

	bool inited;

  SDL_Surface* windowSurface;

  int width, height;
} Graphics;

bool initGraphics(Graphics*, int w, int h);

void updateGraphics(Graphics* g, uint8_t* vram, bool debug);

void destroyGraphics(Graphics*);
