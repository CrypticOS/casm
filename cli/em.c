#include <stdio.h>
#include <stdlib.h>
#include "options.h"

#if EMULATOR_WINDOW == 1
	#include "gfx/gfx.h"
#endif

void run(char *input, char *keys) {
#if EMULATOR_WINDOW == 1
	struct gfx_window window = gfx_open(640, 480, "CrypticOS Emulator");
	struct gfx_interaction ia;
	gfx_setColor(&window, 255, 0, 0);
#endif
		
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

#if !EMULATOR_WINDOW
	size_t get = 0;
#endif
	for (int c = 0; input[c] != '\0'; c++) {
		switch (input[c]) {
		case ',':
#if EMULATOR_WINDOW == 1
			ia = gfx_event();
			while (ia.type != KEY) {
				ia = gfx_event(&window);
			}

			membottom[bottomp] = ia.value;
#else
			if (EMULATOR_USE_KEYS) {
				// Switch between regular and raw input modes.
				system("/bin/stty raw");
				membottom[bottomp] = getchar();
				system("/bin/stty cooked");
			} else {
				if (keys[get] == '\0') {
					puts("Read outside input, stopping\n");
					free(memtop);
					free(membottom);
					free(labels);
					return;
				}

				membottom[bottomp] = keys[get];
				get++;
			}
#endif
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
#if EMULATOR_WINDOW == 1
			// Manage standard OUT instructions, for graphics.
			switch (memtop[topp]) {
			case 0: // WRITE_PIXEL
				gfx_pixel(&window, memtop[topp + 1], memtop[topp + 2]);
				break;
			case 1: // SET_COLOR
				gfx_setColor(&window, memtop[topp + 1], memtop[topp + 2], memtop[topp + 3]);
				break;
			}
#else
			putchar(membottom[bottomp]);
#endif	
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

	// Handle end of window event
#if EMULATOR_WINDOW == 1
	puts("Program finished. Press Q to exit.");
	while (1) {
		ia = gfx_event(&window);
		if (ia.type == KEY && ia.value == 'q') {
			break;
		}
	}
#endif

	free(membottom);
	free(memtop);
	free(labels);
}
