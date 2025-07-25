#include "ppudecode.h"

void printTileBitPair(uint8_t bitPair) {
    if (bitPair == 0b11)
    {
      printf("\033[42m11\033[0m");
    } else if (bitPair == 0b10)
    {
      printf("\033[48;5;46m10\033[0m");
    } else if (bitPair == 0b01)
    {
      printf("\033[48;5;22m01\033[0m");
    } else if (bitPair == 0b00)
    {
      printf("00");
    }
}

void printPixelPallettes(uint8_t* tileData, size_t tileAmount, uint8_t* palletteOut, bool verbose)
{
  // ur welcome for the magic numbers
  for (size_t tileIndex = 0; tileIndex < tileAmount; tileIndex++)
  {
    // 64 total pixels in a tile, 16 bytes
    // uint8_t outputPallette[64];

    if (verbose) printf("  Parsing tile %ld\n", tileIndex);

    for (size_t tileByte = 0; tileByte < 16; tileByte += 2)
    {
      size_t byteIndex = (tileIndex * 16) + tileByte;
      // processing the entire tile here
      uint16_t line = 0;
      uint8_t pixels[8]; // the pixels in the current line containing two bits
      for (int bit = 7; bit >= 0; bit--)
      {
        //tileData[byteIndex+1] is the hb of the 2bpp
        line |= ((tileData[byteIndex] & (1 << bit)) + ((tileData[byteIndex + 1] & (1 << bit)) << 1)) << bit;
      }

      for (int bit = 0; bit < 8; bit++)
      {
        pixels[7-bit] = (line >> (bit * 2)) & 0b11;
      }

      for (size_t bit = 0; bit < 8; bit++)
      {
        // outputPallette[(tileByte / 2) * 8 + bit] = pixels[bit];
        palletteOut[tileIndex * 64 + (tileByte / 2) * 8 + bit] = pixels[bit];

        if (verbose) printTileBitPair(pixels[bit]);
      }
      if (verbose) printf("\n");
    }

    // // Example on retreiving from outputPallette
    //
    // printf("\n");
    // printf(" Output pallete\n");
    // for (size_t y = 0; y < 8; y++)
    // {
    //   for (size_t x = 0; x < 8; x++)
    //   {
    //     printTileBit(outputPallette[y * 8 + x]);
    //   }
    //   printf("\n");
    // }

  }
}
