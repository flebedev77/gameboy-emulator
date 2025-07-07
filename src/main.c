#include "gameboy.h"

int main()
{
	Gameboy gb = {};
	gb.flags = VERBOSE | DEBUG;

	initGameboy(&gb, "testroms/tetris.gb");

	runGameboy(&gb);

	destroyGameboy(&gb);
	return 0;
}
