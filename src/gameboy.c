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

uint8_t fetch(Gameboy* gb)
{
	uint8_t byte = gb->memory[gb->cpu.PC];
	gb->cpu.PC++;
	gb->cpu.cycles++;
	return byte;
}

void executeInstruction(Gameboy* gb)
{
	bool debug = gb->flags & DEBUG;

	uint8_t ins = fetch(gb);	

	if (debug) printf(" %x ", ins);

	switch(ins)
	{
		case 0xC3: // JP a16
			uint16_t addrBytes[2];
			addrBytes[0] = fetch(gb);
			addrBytes[1] = fetch(gb);
			uint16_t addr = (addrBytes[1] << 8) + addrBytes[0];
			gb->cpu.PC = addr;
			gb->cpu.cycles += 16;

			if (debug) printf("JP  $%x\n", addr);
			break;
		case 0x3E: // LDA n8
			uint8_t value = fetch(gb);	
			gb->cpu.A = value;
			gb->cpu.cycles += 8;

			if (debug) printf("LDA $%x\n", value);
			break;
		default:
			if (debug) printf("Unknown opcode %x\n", ins);
			break;
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

	size_t executeAmountInstructions = 10;
	for (size_t i = 0; i < executeAmountInstructions; i++)
	{
		executeInstruction(gb);
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
