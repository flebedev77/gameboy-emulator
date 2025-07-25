#pragma once
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "graphics.h"
#include "ppudecode.h"
#include "hardwareinc.h"

#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 144

#define MEMORY_SIZE 0x10000
#define MEMORY_DUMPFILENAME "memory.bin"

#define GAMEBOY_CLOCK_SPEED_HZ 4194304
#define MS_PER_CYCLE (1000 / GAMEBOY_CLOCK_SPEED_HZ)

#define CPU_DUMPFILENAME "cpu.bin"

#define VRAM_BEGIN 0x8000
#define VRAM_END 0x9FFF
#define VRAM_AMOUNT (VRAM_END - VRAM_BEGIN)
#define VRAM_TILES_AMOUNT VRAM_AMOUNT / 16

enum GameboyFlags
{
	INITED = 1 << 0,
	VERBOSE = 1 << 1,
	DEBUG = 1 << 2,
	GRAPHICS_DISABLED = 1 << 3
};

enum CPUFlags
{
	Z = 1 << 7, // Zero
	N = 1 << 6, // Subtract
	H = 1 << 5, // Half carry
	C = 1 << 4  // Carry
};

typedef struct 
{
	uint16_t PC;
	uint16_t SP; 	

	uint8_t A,
					B,
					C,
					D,
					E,
					H,
					L;

	uint8_t F;

	bool IME;

	uint32_t cycles;
} CPU;

typedef struct 
{
	FileData rom; 
	int flags;

	uint8_t* memory;
  uint8_t* vram;

	CPU cpu;

	Graphics graphics;
} Gameboy;

void initGameboy(Gameboy*, const char* romFilename);

void runGameboy(Gameboy*);

void destroyGameboy(Gameboy*);
