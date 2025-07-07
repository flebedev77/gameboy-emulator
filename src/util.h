#pragma once
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  uint8_t* data;
  size_t len;
} FileData;

FileData readFile(const char*);
