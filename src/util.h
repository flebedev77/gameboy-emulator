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

FileData readFile(const char*);
bool     writeFile(const char*, uint8_t*, size_t);

void sleepMs(int);
