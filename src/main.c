#include "gameboy.h"

int main()
{
  Gameboy gb = {};
  gb.flags |= VERBOSE;

  initGameboy(&gb, "testroms/basic.gb");

  runGameboy(&gb);

	destroyGameboy(&gb);
  return 0;
}
