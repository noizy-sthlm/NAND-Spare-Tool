CFLAGS = -Wall -Wextra -std=c11 -D_POSIX_C_SOURCE=200809L

# Default target: build the executable
all:
	gcc $(CFLAGS) src/nand-tool.c -o nand_tool

# Clean target: remove the executable
clean:
	rm -f nand_tool
