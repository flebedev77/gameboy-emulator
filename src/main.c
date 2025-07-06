#include "gameboy.h"

int main()
{
  Gameboy gb;
  gb.flags |= VERBOSE;

  initGameboy(&gb, "testroms/tetris.gb");
  runGameboy(&gb);

  return 0;
}
