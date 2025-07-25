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

void sleepMs(int milliseconds) {
#ifdef _WIN32
    Sleep(milliseconds); // Windows
#elif defined(__EMSCRIPTEN__)
    emscripten_sleep(milliseconds); // WASM
#else
    usleep(milliseconds * 1000); // POSIX (Linux, macOS)
#endif
}

uint8_t** allocateMatrix(size_t rows, size_t cols) {
    uint8_t** matrix = malloc(rows * sizeof(uint8_t*));
    for (size_t i = 0; i < rows; i++) {
        matrix[i] = malloc(cols * sizeof(uint8_t));
    }
    
    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            matrix[i][j] = i * cols + j + 1; 
        }
    }

    return matrix;
}

void freeMatrix(uint8_t** matrix, size_t rows) {
    for (size_t i = 0; i < rows; i++) {
        free(matrix[i]); 
    }
    free(matrix); 
}
