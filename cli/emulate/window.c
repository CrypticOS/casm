// A drop in replacement for terminal.c

#include <stdio.h>
#include <stdlib.h>
#include "gfx/gfx.h"
#include "../options.h"

#define MAX_INPUT 1000
#define MAX_TOP 100
#define MAX_BOTTOM 10000
#define MAX_LABELS 1000

void run(char *input, char *keys) {
	struct gfx_window window = gfx_open(500, 500, "CrypticOS Emulator");
	struct gfx_interaction ia;
	gfx_setColor(&window, 255, 0, 0);
	
	unsigned short *memtop = malloc(sizeof(unsigned short) * MAX_TOP);
	unsigned short *membottom = malloc(sizeof(unsigned short) * MAX_BOTTOM);

	size_t topp = 0;
	size_t bottomp = 0;

	// Locate Labels
	size_t *labels = malloc(sizeof(size_t) * MAX_LABELS);
	int l = 0;
	for (int c = 0; input[c] != '\0'; c++) {
		if (input[c] == '|') {
			labels[l] = c;
			l++;
		}
	}

	for (int c = 0; input[c] != '\0'; c++) {
		switch (input[c]) {
		case ',':
			ia = gfx_event();
			while (ia.type == KEY) {
				ia = gfx_event(&window);
			}

			membottom[bottomp] = ia.value;

			break;
		case '!':
			membottom[bottomp] = 0;
			break;
		case '%':
			membottom[bottomp] += 50;
			break;
		case '*':
			membottom[bottomp] += 5;
			break;
		case '+':
			membottom[bottomp]++;
			break;
		case '-':
			membottom[bottomp]--;
			break;
		case '.':
			// Manage standard OUT instructions, for graphics.
			switch (memtop[topp]) {
			case 0: // WRITE_PIXEL
				gfx_pixel(&window, memtop[topp + 1], memtop[topp + 2]);
				break;
			case 1: // SET_COLOR
				gfx_setColor(&window, memtop[topp + 1], memtop[topp + 2], memtop[topp + 3]);
				break;
			}
			
			break;
		case '>':
			bottomp++;
			break;
		case '<':
			bottomp--;
			break;
		case 'd':
			topp++;
			break;
		case 'a':
			topp--;
			break;
		case '^':
			memtop[topp] = membottom[bottomp];
			break;
		case 'v':
			membottom[bottomp] = memtop[topp];
			break;
		case '$':
			c = labels[memtop[topp] - 1];
			break;
		case '?':
			if (memtop[topp + 1] == memtop[topp + 2]) {
				c = labels[memtop[topp] - 1];
			}

			break;

		// Debug char
		case '#':
			break;
		}
	}

	puts("Program finished. Press Q to exit.");
	while (ia.type != KEY && ia.value != 'q') {
		ia = gfx_event(&window);
	}

	free(membottom);
	free(memtop);
	free(labels);
}
