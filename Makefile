chip8: main.c cpu.c
	gcc -g -o chip8 -Wall main.c cpu.c `pkg-config --cflags --libs sdl2` 