#include "gameboy.h"

void initGameboy(Gameboy* gb, const char* romFilename)
{
  if (gb == NULL)
  {
    perror("Gameboy passed was invalid");    
    return;
  }
  gb->flags |= INITED;

  gb->rom = readFile(romFilename);
  if (gb->flags & VERBOSE)
  {
    printf("ROM: %ld bytes read from %s\n", gb->rom.len, romFilename);
  }

}

void runGameboy(Gameboy* gb)
{
  if (gb == NULL)
  {
    perror("Gameboy passed was invalid");
    return;
  }

  if (!(gb->flags & INITED))
  {
    perror("Gameboy was not initialized");
    return;
  }

}
