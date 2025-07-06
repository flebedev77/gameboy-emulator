#pragma once
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  unsigned char* data;
  size_t len;
} FileData;

FileData readFile(const char*);
