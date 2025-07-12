#include "gameboy.h"

void initGameboy(Gameboy* gb, const char* romFilename)
{
	if (gb == NULL)
	{
		perror("Gameboy passed was invalid");    
		return;
	}

	gb->flags |= INITED;


	if (!(gb->flags & GRAPHICS_DISABLED))
	{
		memset(&gb->graphics, 0, sizeof(Graphics));
		if (initGraphics(&gb->graphics, SCREEN_WIDTH, SCREEN_HEIGHT) > 0)
		{
			perror("Could not initialise graphics backend: SDL\n");
			return;
		}
	}

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

	gb->cpu.IME = 1;

	gb->memory = malloc(MEMORY_SIZE);
	memset(gb->memory, 0, MEMORY_SIZE);

	gb->memory[0xFF44] = 148; // Gaslight the program into thinking we in vblank

	if (verbose)
		printf("Copying rom (%ld bytes) into memory map (%d bytes)\n", gb->rom.len, MEMORY_SIZE);
	for (size_t i = 0; i < gb->rom.len; i++)
	{
		gb->memory[i] = gb->rom.data[i];
	}

	if (verbose)
		printf("Initialisation complete\n");

}

void performDump(Gameboy* gb)
{
	printf("Writing memory dump to %s\n", MEMORY_DUMPFILENAME);
	writeFile(MEMORY_DUMPFILENAME, gb->memory, MEMORY_SIZE);

	printf("Writing cpu dump to %s\n", CPU_DUMPFILENAME);
	char buf[142];
	snprintf(buf, sizeof(buf), "A  = $%02X\nB  = $%02X\nC  = $%02X\nD  = $%02X\nE  = $%02X\nH  = $%02X\nL  = $%02X\nF  = $%02X\nPC = $%04X\nSP = $%04X\n",
			gb->cpu.A,
			gb->cpu.B,
			gb->cpu.C,
			gb->cpu.D,
			gb->cpu.E,
			gb->cpu.H,
			gb->cpu.L,
			gb->cpu.F,
			gb->cpu.PC,
			gb->cpu.SP			
			);
	writeFile(CPU_DUMPFILENAME, (uint8_t*)buf, 0x5F); // My dumbass can't calculate the length of a string properly
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

void writeMemory(Gameboy* gb, uint16_t addr, uint8_t value)
{
	gb->memory[addr] = value;
	gb->cpu.cycles++;	
}
uint8_t readMemory(Gameboy* gb, uint16_t addr)
{
	gb->cpu.cycles++;
	return gb->memory[addr];
}

void pushStack(Gameboy* gb, uint8_t value)
{
	gb->cpu.SP--; // Subtract before setting value, because the stack grows downwards, and the SP points to the latest value
	
	writeMemory(gb, gb->cpu.SP, value);
}

uint8_t popStack(Gameboy* gb)
{
	uint8_t value = readMemory(gb, gb->cpu.SP);
	gb->cpu.SP++;
	return value;
}

void pushStackWord(Gameboy* gb, uint16_t value)
{
	uint8_t lb, hb;
	wordToBytes(&hb, &lb, value);
	pushStack(gb, hb); // Stack here in reverse order of the endianess
	pushStack(gb, lb);
}

uint16_t popStackWord(Gameboy* gb)
{
	uint8_t lb = popStack(gb);
	uint8_t hb = popStack(gb);
	return bytesToWord(hb, lb);
}

void setZeroflag(Gameboy* gb, uint8_t value)
{
	if (value == 0)
		gb->cpu.F |= Z;
	else
		gb->cpu.F &= ~Z;
}

void addRegister(Gameboy* gb, uint8_t* reg, uint8_t value)
{
	uint16_t result = (uint16_t)*reg + (uint16_t)value; 
	uint8_t carryPerBit[8]; 

	*reg = (uint8_t)result;

	
	for (int i = 0; i < 8; i++) {
		carryPerBit[i] = ((*reg >> i) & 1) + ((value >> i) & 1) < 0 ? 1 : 0;
	}
	
	setZeroflag(gb, result);
	
	gb->cpu.F |= N;

	if (carryPerBit[3])
		gb->cpu.F |= H;
	else 
		gb->cpu.F &= ~H;

	if (carryPerBit[7])
		gb->cpu.F |= C;
	else
		gb->cpu.F &= ~carryPerBit[7];	
}

void subRegister(Gameboy* gb, uint8_t* reg, uint8_t value)
{
	uint16_t result = (uint16_t)*reg - (uint16_t)value; 
	uint8_t carryPerBit[8]; 

	*reg = (uint8_t)result;

	
	for (int i = 0; i < 8; i++) {
		carryPerBit[i] = ((*reg >> i) & 1) - ((value >> i) & 1) < 0 ? 1 : 0;
	}

	setZeroflag(gb, result);

	gb->cpu.F |= N;

	if (carryPerBit[3])
		gb->cpu.F |= H;
	else 
		gb->cpu.F &= ~H;

	if (carryPerBit[7])
		gb->cpu.F |= C;
	else
		gb->cpu.F &= ~carryPerBit[7];	
}

void orRegister(Gameboy* gb, uint8_t* reg)
{
	gb->cpu.A = gb->cpu.A | *reg;	

	if (gb->cpu.A == 0)
		gb->cpu.F |= Z;
	else
		gb->cpu.F &= ~Z;

	gb->cpu.F &= ~(N | H | C);
}

uint8_t fetch(Gameboy* gb)
{
	uint8_t byte = gb->memory[gb->cpu.PC];
	gb->cpu.PC++;
	gb->cpu.cycles++;
	return byte;
}

uint8_t fetchStack(Gameboy* gb)
{
	uint8_t byte = gb->memory[gb->cpu.SP];
	gb->cpu.SP--;
	gb->cpu.cycles++;
	return byte;
}

uint16_t fetchWord(Gameboy* gb)
{ // fetches two bytes, swaps them around and adds them
	return bytesToWord(fetch(gb), fetch(gb));
}

uint16_t fetchWordStack(Gameboy* gb)
{
	return bytesToWord(fetchStack(gb), fetchStack(gb));
}

void executeInstruction(Gameboy* gb)
{
	bool debug = gb->flags & DEBUG;

	uint8_t ins = fetch(gb);	

	uint16_t addr = 0x0000;
	uint16_t word = 0x0000;
	int8_t raddr = 0x00;
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
		case 0x20: // JR NZ, e8
			raddr = (int8_t)fetch(gb);
				
			if (!(gb->cpu.F & Z))
			{
				gb->cpu.PC += raddr;
				gb->cpu.cycles += 10;
			} else
			{
				gb->cpu.cycles += 6;
			}

			if (debug) printf("JR NZ, %d\n", raddr);
			break;
		case 0xCD: // CALL a16
			addr = fetchWord(gb);
			
			pushStackWord(gb, gb->cpu.PC);	

			gb->cpu.PC = addr;

			if (debug) printf("CALL $%04X\n", addr);
			break;
		case 0xC9: // RET
			gb->cpu.PC = popStackWord(gb);

			if (debug) printf("RET        (JP $%04X)\n", gb->cpu.PC);
			break;
		case 0xE2: // POP HL
			wordToBytes(&gb->cpu.H, &gb->cpu.L, fetchWordStack(gb));	

			if (debug) printf("POP HL        (HL=$%04X,SP=$%04X)\n", bytesToWord(gb->cpu.H, gb->cpu.L), gb->cpu.SP);
			break;
		case 0xFE: // CP n8
			value = fetch(gb);
			uint8_t A = gb->cpu.A;

			subRegister(gb, &gb->cpu.A, value);

			gb->cpu.A = A;

			if (debug) printf("CP $%02X         (CP $%02X, $%02X)\n", value, A, value);
			break;
		case 0xAF: // XOR A, A
			gb->cpu.A ^= gb->cpu.A;

			if (gb->cpu.A == 0)
				gb->cpu.F |= Z;
			else
				gb->cpu.F &= ~Z;	

			gb->cpu.F &= ~(N | H | C);

			gb->cpu.cycles += 3;	
			if (debug) printf("XOR A, A\n");
			break;
		case 0xB1: // OR A, C
			value = gb->cpu.A;
			orRegister(gb, &gb->cpu.C);

			if (debug) printf("OR A, C         (OR $%02X $%02X)\n", value, gb->cpu.C);
			break;
		case 0xE6: // AND A, n8
			value = fetch(gb);
			gb->cpu.A = gb->cpu.A & value;

			setZeroflag(gb, gb->cpu.A);
			gb->cpu.F |= H;
			gb->cpu.F &= ~(N | C);
			
			if (debug) printf("AND A, %02X\n", value);
			break;
		case 0x3E: // LDA n8
			value = fetch(gb);	
			gb->cpu.A = value;
			gb->cpu.cycles += 7;

			if (debug) printf("LDA $%02X\n", value);
			break;
		case 0x78: // LD A, B
			gb->cpu.A = gb->cpu.B;

			if (debug) printf("LD A, B         (LDA $%02X)\n", gb->cpu.B);
			break;	
		case 0x21: // LD HL, n16
			word = fetchWord(gb);
			wordToBytes(&gb->cpu.H, &gb->cpu.L, word);

			gb->cpu.cycles += 9; // 1 is used to read the op code, two are used to read the word. This instruction is meant to take 12 cycles
			if (debug) printf("LD HL, $%04X\n", word);
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
		case 0x01: // LD BC, n16
			word = fetchWord(gb);
			wordToBytes(&gb->cpu.B, &gb->cpu.C, word);

			if (debug) printf("LD BC, $%04X\n", word);
			break;
		case 0x32: // LD [HL-], A
			addr = bytesToWord(gb->cpu.H, gb->cpu.L);
			writeMemory(gb, addr, gb->cpu.A);

			wordToBytes(&gb->cpu.H, &gb->cpu.L, addr-1); 

			gb->cpu.cycles += 6;	
			if (debug) printf("LD [HL-], A   (LD $%04X, $%02X)\n", addr, gb->cpu.A);
			break;
		case 0x2A: // LD A, [HL+]
			addr = bytesToWord(gb->cpu.H, gb->cpu.L);
			gb->cpu.A = readMemory(gb, addr);

			wordToBytes(&gb->cpu.H, &gb->cpu.L, addr+1);

			if (debug) printf("LD A, [HL+]   (LDA $%02X)\n", gb->cpu.A);
			break;
		case 0x36: // LD [HL], n8
			value = fetch(gb);
			addr = bytesToWord(gb->cpu.H, gb->cpu.L);
			writeMemory(gb, addr, value);

			gb->cpu.cycles += 2;
			if (debug) printf("LD [HL], $%02X  (LD $%04X, $%02X)\n", value, addr, value);
			break;
		case 0xEA: // LD [a16], A
			addr = fetchWord(gb);
			writeMemory(gb, addr, gb->cpu.A);

			if (debug) printf("LD [$%04X], A (LD $%04X, $%02X)\n", addr, addr, gb->cpu.A);
			break;
		case 0x31: // LD SP, n16
			addr = fetchWord(gb);
			gb->cpu.SP = addr;
			gb->cpu.cycles += 3;
			if (debug) printf("LD SP, $%04X\n", addr);
			break;
		case 0xE0: // LDH [a8], A
			value = fetch(gb);	
			addr = bytesToWord(0xFF, value);
			writeMemory(gb, addr, gb->cpu.A);
			
			if (debug) printf("LDH [$%04X], A\n", addr);
			break;
		case 0xF0: // LDH A, [a8]
			value = fetch(gb);
			addr = bytesToWord(0xFF, value);	
			gb->cpu.A = readMemory(gb, addr);

			if (debug) printf("LDH A, [$%04X]\n", addr);
			break;	
		case 0x05: // DEC B
			value = gb->cpu.B;
			subRegister(gb, &gb->cpu.B, 1);	
			
			if (debug) printf("DEC B         (DEC $%02X)\n", value);
			break;
		case 0x0D: // DEC C
			value = gb->cpu.C;
			subRegister(gb, &gb->cpu.C, 1);
			
			if (debug) printf("DEC C         (DEC $%02X)\n", value);
			break;
		case 0x0B: // DEC BC
			word = bytesToWord(gb->cpu.B, gb->cpu.C);
			// Apparently we can just do this without setting any flags, according to gekko.fi'spseudocode
			wordToBytes(&gb->cpu.B, &gb->cpu.B, word-1);

			if (debug) printf("DEC BC         (DEC $%04X)\n", word);
			break;
		case 0x0C: // INC C
			value = gb->cpu.C;
			addRegister(gb, &gb->cpu.C, 1);

			if (debug) printf("INC C         (INC $%02X)\n", value);
			break;
		case 0x2F: // CPL
			gb->cpu.A = ~gb->cpu.A;
			gb->cpu.F |= (Z | H);

			if (debug) printf("CPL           (CPL $%02X)\n", gb->cpu.A);
			break;
		case 0xF3: // DI
			gb->cpu.IME = 0;
			gb->cpu.cycles += 3;

			if (debug) printf("DI\n");
			break;
		case 0xFB: // EI
			gb->cpu.IME = 1;
			gb->cpu.cycles += 2;
			
			if (debug) printf("EI\n");
			break;

		default:
			if (debug) printf("Unknown opcode %02X\n", ins);
			break;
	}
}

void executePPUCycle(Gameboy* gb)
{
	// TODO: write out everything to a texture in the graphics struct
	
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

	updateGraphics(&gb->graphics);
	size_t executeAmountInstructions = 5000000;//(0x00FF * 3) * 0x40;
	for (size_t i = 0; i < executeAmountInstructions; i++)
	{
		executeInstruction(gb);
	}
	
	/*
	while (!gb->graphics.shouldQuit)
	{
		executeInstruction(gb);

		if (!(gb->flags & GRAPHICS_DISABLED))
			updateGraphics(&gb->graphics);

		sleepMs(MS_PER_CYCLE);
	}
	*/

	if (debug)
	{
		performDump(gb);
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

	if (!(gb->flags & GRAPHICS_DISABLED))
		destroyGraphics(&gb->graphics);
}
