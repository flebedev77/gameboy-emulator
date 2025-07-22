CC = gcc

CFLAGS = -Wall -Wextra -g
LINK_FLAGS = -lSDL2

SRCS = src/main.c src/util.c src/gameboy.c src/graphics.c src/ppudecode.c

OBJ_DIR = build
OBJS = $(patsubst src/%.c, $(OBJ_DIR)/%.o, $(SRCS))

TARGET = $(OBJ_DIR)/gbemu

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LINK_FLAGS)

$(OBJ_DIR)/%.o: src/%.c
	@mkdir -p $(OBJ_DIR)  
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

.PHONY: all clean
