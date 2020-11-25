#include <stdio.h>
#include <stdlib.h>
#include "options.h"

#define MAX_INPUT 1000
#define MAX_TOP 100
#define MAX_BOTTOM 10000
#define MAX_LABELS 1000

// Perform emulator system call
void syscall() {
	
}

// 16 bit emulator
void run(char *input, char *keys) {
	putchar(13); // Carriage return
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

	size_t get = 0;
	for (int c = 0; input[c] != '\0'; c++) {
		switch (input[c]) {
		case ',':
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
			putchar(membottom[bottomp]);
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
			printf("[%d - %d]\n", c, bottomp);
			break;
		}
	}

	free(membottom);
	free(memtop);
	free(labels);

	putchar('\n');
}
