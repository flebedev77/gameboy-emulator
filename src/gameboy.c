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

	bool verbose = gb->flags & VERBOSE;
  if (verbose)
    printf("ROM: %ld bytes read from %s\n", gb->rom.len, romFilename);

	gb->cpu = (CPU){};
	gb->cpu.PC = 0x0100;
	gb->cpu.SP = 0xFFFE;

	gb->cpu.A = 0x01;
	gb->cpu.F = 0x00;
	gb->cpu.B = 0xFF;
	gb->cpu.C = 0x13;
	gb->cpu.D = 0x00;
	gb->cpu.E = 0xC1;
	gb->cpu.H = 0x84;
	gb->cpu.L = 0x03;

	gb->memory = malloc(MEMORY_SIZE);
	memset(gb->memory, 0, MEMORY_SIZE);

	if (verbose)
		printf("Copying rom (%ld bytes) into memory map (%d bytes)\n", gb->rom.len, MEMORY_SIZE);
	for (size_t i = 0; i < gb->rom.len; i++)
	{
		gb->memory[i] = gb->rom.data[i];
	}

	if (verbose)
		printf("Initialisation complete\n");
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

void destroyGameboy(Gameboy* gb)
{
	if (gb == NULL)
	{
		perror("Gameboy passed was invalid");
		return;
	}

	free(gb->rom.data);
	free(gb->memory);
}
