#pragma once
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

#define MEMORY_SIZE 0x10000
#define MEMORY_DUMPFILENAME "memory.bin"

enum GameboyFlags
{
  INITED = 1 << 0,
  VERBOSE = 1 << 1,
	DEBUG = 1 << 2
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

	uint32_t cycles;
} CPU;

typedef struct 
{
  FileData rom; 
  int flags;

	uint8_t* memory;

	CPU cpu;
} Gameboy;

void initGameboy(Gameboy*, const char* romFilename);

void runGameboy(Gameboy*);

void destroyGameboy(Gameboy*);
