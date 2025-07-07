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

uint16_t bytesToWord(uint8_t H, uint8_t L)
{
	return (H << 8) + L;
}

void wordToBytes(uint8_t* H, uint8_t* L, uint16_t word)
{
	*L = word & 0x00FF;
	*H = (word >> 8) & 0x00FF; 
}

uint16_t fetchWord(Gameboy* gb)
{ // fetches two bytes, swaps them around and adds them
	return bytesToWord(fetch(gb), fetch(gb));
}

void executeInstruction(Gameboy* gb)
{
	bool debug = gb->flags & DEBUG;

	uint8_t ins = fetch(gb);	

	uint16_t addr = 0x0000;
	uint8_t value = 0x00;

	if (debug) printf(" %02X $%04X ", ins, gb->cpu.PC-1);

	switch(ins)
	{
		case 0x00: // NOP
			if (debug) printf("NOP\n");
			break;
		case 0xC3: // JP a16
			addr = fetchWord(gb);
			gb->cpu.PC = addr;
			gb->cpu.cycles += 13;

			if (debug) printf("JP  $%04X\n", addr);
			break;
		case 0x3E: // LDA n8
			value = fetch(gb);	
			gb->cpu.A = value;
			gb->cpu.cycles += 7;

			if (debug) printf("LDA $%04X\n", value);
			break;
		case 0xAF: // XOR A, A
			gb->cpu.A ^= gb->cpu.A;

			if (gb->cpu.A == 0)
			{
				gb->cpu.F |= Z;
			} else
			{
				gb->cpu.F &= ~Z;	
			}
			gb->cpu.F &= ~(N | H | C);

			gb->cpu.cycles += 3;	
			if (debug) printf("XOR A, A\n");
			break;
		case 0x21: // LD HL, n16
			uint8_t bytes[2];
			bytes[0] = fetch(gb);
			bytes[1] = fetch(gb);

			gb->cpu.H = bytes[1];
			gb->cpu.L = bytes[0];

			gb->cpu.cycles += 9; // 1 is used to read the op code, two are used to read the word. This instruction is meant to take 12 cycles
			if (debug) printf("LD HL, $%02X%02X\n", bytes[1], bytes[0]);
			break;
		case 0x0E: // LDC n8
			value = fetch(gb);
			gb->cpu.C = value;

			gb->cpu.cycles += 6;
			if (debug) printf("LDC $%02X\n", value);
			break;
		case 0x06: // LDB n8
			value = fetch(gb);
			gb->cpu.B = value;

			gb->cpu.cycles += 6;
			if (debug) printf("LDB $%02X\n", value);
			break;
		case 0x32: // LD [HL-], A
			addr = bytesToWord(gb->cpu.H, gb->cpu.L);
			gb->memory[addr] = gb->cpu.A;	

			wordToBytes(&gb->cpu.H, &gb->cpu.L, addr-1); 

			gb->cpu.cycles += 7;	
			if (debug) printf("LD [HL-], A   (LD $%04X, $%02X)\n", addr, gb->cpu.A);
			break;

		default:
			if (debug) printf("Unknown opcode %02X\n", ins);
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

	bool debug = gb->flags & DEBUG;

	if (debug)
	{
		printf("\nCPU execution log\n");
		printf(" OP  MEM  ASEM              (EVAL) \n");
	}

	size_t executeAmountInstructions = 10;
	for (size_t i = 0; i < executeAmountInstructions; i++)
	{
		executeInstruction(gb);
	}

	if (debug)
	{
		printf("Writing memory dump to %s\n", MEMORY_DUMPFILENAME);
		writeFile(MEMORY_DUMPFILENAME, gb->memory, MEMORY_SIZE);
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
