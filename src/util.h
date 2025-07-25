#pragma once
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#elif defined(__EMSCRIPTEN__)
#include <emscripten.h>
#else
#include <unistd.h>
#endif

typedef struct {
	uint8_t* data;
	size_t len;
} FileData;

FileData readFile(const char* filename);
bool     writeFile(const char* filename, uint8_t* data, size_t dataLen);

void sleepMs(int duration);

uint8_t** allocateMatrix(size_t rows, size_t cols);
void freeMatrix(uint8_t** matrix, size_t rows);
