#pragma once
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "globals.h"

uint8_t mapPalletteToColor(uint8_t pixel);

void printPixelPallettes(uint8_t* tileData, size_t tileAmount, uint8_t* palletteOut, bool verbose);
