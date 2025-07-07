#include "util.h"

FileData readFile(const char* filename)
{
  FILE* file = fopen(filename, "rb");
  
  if (file == NULL)
  {
    perror("Unable to open file");
    return (FileData){};
  }

  fseek(file, 0, SEEK_END);
  long fileSize = ftell(file);
  fseek(file, 0, SEEK_SET);

  unsigned char* fileData = malloc(fileSize);
  if (fileData == NULL)
  {
    perror("Unable to allocate byte array for file");
    fclose(file);
    return (FileData){};
  }

  size_t bytesRead = fread(fileData, 1, fileSize, file);
  if ((long)bytesRead != fileSize)
  {
    perror("Error reading from file");
    free(fileData);
    fclose(file);
    return (FileData){};
  }

  return (FileData){fileData, bytesRead};
}

bool writeFile(const char* filename, uint8_t* data, size_t dataLen)
{
	FILE* file = fopen(filename, "wb");
	
	if (file == NULL)
	{
		perror("Error opening file");
		return 1;
	}	
	
	size_t written = fwrite(data, sizeof(uint8_t), dataLen, file);
	if (written != dataLen)
	{
		perror("Error writing to file");
		return 1;
	}

	fclose(file);

	return 0;
}
