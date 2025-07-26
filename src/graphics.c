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
    SDL_DestroyWindow(g->window);
		SDL_Quit();
		return 1;
	}

  int windowSurfaceSize = VRAM_TILEMAP_WIDTH * VRAM_SINGLE_TILE_WIDTH;
  g->windowSurface = SDL_CreateRGBSurface(0, windowSurfaceSize, windowSurfaceSize, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
  if (g->windowSurface == NULL)
  {
    printf("SDL: window surface cloud not be created: %s\n", SDL_GetError());
    SDL_DestroyRenderer(g->renderer);
    SDL_DestroyWindow(g->window);
    SDL_Quit();
    return 1;
  }

	g->inited = true;

  g->width = width;
  g->height = height;

	g->shouldQuit = false;

	return 0;
}

void updateGraphics(Graphics* g, uint8_t* vram, bool verbose)
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

  // uint8_t pallette[VRAM_TILES_AMOUNT * VRAM_SINGLE_TILE_LEN];
  // printPixelPallettes(vram, VRAM_TILES_AMOUNT, &pallette[0], verbose);

  if (verbose)
    printf("                                              Tilemap:\n");

  uint32_t* pixels = (uint32_t*)g->windowSurface->pixels; 
  for (int y = 0; y < VRAM_TILEMAP_WIDTH; y++)
  {
    for (int x = 0; x < VRAM_TILEMAP_WIDTH; x++)
    {
      size_t index = (y * VRAM_TILEMAP_WIDTH) + x;
      uint8_t tileIndex = vram[VRAM_TILEMAP_BEGIN + index];

      // if (verbose) printf("%02X ", tileIndex);

      if (tileIndex != 0)
      {
        uint8_t tilePallette[VRAM_SINGLE_TILE_LEN];
        printPixelPallettes(vram + VRAM_TILEDATA_BEGIN + sizeof(uint8_t) * tileIndex, 1, &tilePallette[0], false);

        for (int ty = 0; ty < VRAM_SINGLE_TILE_WIDTH; ty++)
        {
          for (int tx = 0; tx < VRAM_SINGLE_TILE_WIDTH; tx++)
          {
            size_t pixelIndex = (index * VRAM_SINGLE_TILE_LEN) + ty * VRAM_SINGLE_TILE_WIDTH + tx;
            uint8_t pixel = tilePallette[ty * VRAM_SINGLE_TILE_WIDTH + tx];
            uint8_t gbColor = mapPalletteToColor(pixel);
            if (verbose) printf("%d", pixel);
            pixels[pixelIndex] = SDL_MapRGB(g->windowSurface->format, gbColor, gbColor, gbColor);
          }
          if (verbose) printf("\n");
        }
        if (verbose) printf("\n\n");
      }
    }
  }

  // for (int i = 0; i < VRAM_TILES_AMOUNT * VRAM_SINGLE_TILE_LEN; i++)
  // {
  //   printf("%02X ", pallette[i]);
  //   if (i % 8 == 0) printf("\n");
  // }

  SDL_Texture* renderTex = SDL_CreateTextureFromSurface(g->renderer, g->windowSurface);
  SDL_RenderCopy(g->renderer, renderTex, NULL, NULL);

	SDL_RenderPresent(g->renderer);
  SDL_DestroyTexture(renderTex);
}

void destroyGraphics(Graphics* g)
{
	SDL_DestroyRenderer(g->renderer);
	SDL_DestroyWindow(g->window);
	SDL_Quit();
}
