chip8: main.c cpu.c
	gcc -Os -o chip8 -Wall main.c cpu.c