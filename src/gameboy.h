#pragma once
#include <stdio.h>
#include <stdbool.h>
#include "util.h"

enum GameboyFlags
{
  INITED = 1 << 0,
  VERBOSE = 1 << 1
};

typedef struct 
{
  FileData rom; 
  int flags;
} Gameboy;

void initGameboy(Gameboy*, const char* romFilename);

void runGameboy(Gameboy*);
