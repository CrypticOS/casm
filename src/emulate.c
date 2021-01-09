#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "data.h"

#if EMULATOR_WINDOW
	#include "gfx/gfx.h"
#endif

void run(char *input, char *keys) {
	#if EMULATOR_WINDOW == 1
		struct gfx_window window = gfx_open(640, 480, "CrypticOS Emulator");
		struct gfx_interaction ia;
		gfx_setColor(&window, 255, 0, 0);
	#endif

	// Somewhat messy allocation code :(
	unsigned short *memtop = malloc(sizeof(unsigned short) * MAX_TOP);
	if (memtop == NULL) {puts("Alloc err"); return;}
	unsigned short *membottom = malloc(sizeof(unsigned short) * MAX_BOTTOM);
	if (membottom == NULL) {free(memtop); puts("Alloc err"); return;}
	size_t *labels = malloc(sizeof(size_t) * MAX_LABELS);
	if (labels == NULL) {free(memtop); free(membottom); puts("Alloc err"); return;}
	
	unsigned short *topp = memtop;
	unsigned short *bottomp = membottom;

	// Locate labels
	size_t l = 0;
	for (size_t c = 0; input[c] != '\0'; c++) {
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
		case '|':
			break;
		case '>':
			bottomp++;
			break;
		case '<':
			bottomp--;
			break;
		case '+':
			(*bottomp)++;
			break;
		case '.':
			#if EMULATOR_WINDOW
				// Manage standard OUT instructions, for graphics.
				switch (*topp) {
				case 0: // WRITE_PIXEL
					gfx_pixel(&window, *(topp + 1), *(topp + 2));
					break;
				case 1: // SET_COLOR
					gfx_setColor(&window, *(topp + 1), *(topp + 2), *(topp + 3));
					break;
				}
			#else
				putchar(*bottomp);
			#endif	
		break;
		case '!':
			*bottomp = 0;
			break;
		case '%':
			*bottomp += 50;
			break;
		case '*':
			*bottomp += 5;
			break;
		case 'd':
			topp++;
			break;
		case 'a':
			topp--;
			break;
		case '^':
			*topp = *bottomp;
			break;
		case '$':
			c = labels[(*topp)];
			break;
		case '?':
			if (*(topp + 1) == *(topp + 2)) {
				c = labels[(*topp)];
			}

			break;
		case 'v':
			*bottomp = *topp;
			break;
		case '-':
			(*bottomp)--;
			break;
		case ',':
			#if EMULATOR_WINDOW
				ia = gfx_event();
				while (ia.type != KEY) {
					ia = gfx_event(&window);
				}

				*bottomp = ia.value;
			#else
				if (keys == NULL) {
					// Switch between regular and raw input modes.
					system("/bin/stty raw");
					*bottomp = getchar();
					system("/bin/stty cooked");
				} else {
					// Don't allow characters after null terminator to be
					// read
					if (get != 0 && keys[get - 1] == '\0') {
						puts("Read outside input, stopping\n");
						free(memtop);
						free(membottom);
						free(labels);
						return;
					}

					*bottomp = keys[get];
					get++;
				}
			#endif
			break;

		// Debug char
		case '#':
			puts("\nHalting program.");
			puts("Dumping 100 memory cells from bottom...");
			for (size_t i = 0; i < 100; i++) {
				putchar(membottom[i]);
			}

			goto endAll;
		}
	}

	// Handle end of window event
	#if EMULATOR_WINDOW
		puts("Program finished. Press Q to exit.");
		while (1) {
			ia = gfx_event(&window);
			if (ia.type == KEY && ia.value == 'q') {
				break;
			}
		}
	#endif


	endAll:
	free(membottom);
	free(memtop);
	free(labels);
}
