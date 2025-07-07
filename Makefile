CC = gcc

CFLAGS = -Wall -Wextra -g

SRCS = src/main.c src/util.c src/gameboy.c

OBJ_DIR = build
OBJS = $(patsubst src/%.c, $(OBJ_DIR)/%.o, $(SRCS))

TARGET = $(OBJ_DIR)/gbemu

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^

$(OBJ_DIR)/%.o: src/%.c
	@mkdir -p $(OBJ_DIR)  
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

.PHONY: all clean
