#include <stdio.h>
#include <stdlib.h>

#define MAX_INPUT 10000
#define MAX_TOP 10000
#define MAX_BOTTOM 10000
#define MAX_LABELS 10000

// Perform emulator system call
void syscall() {
	
}

// 16 bit emulator
int run(char *input, char *keys) {
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

	int get = 0;
	for (int c = 0; input[c] != '\0'; c++) {
		//printf("##%d - %d\n", c, bottomp);
		switch (input[c]) {
		case ',':
			if (keys[get] == '\0') {
				puts("Read outside input, stopping\n");
				return -1;
			}

			membottom[bottomp] = keys[get];
			get++;
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
		}
	}

	free(membottom);
	free(memtop);
	free(labels);

	putchar('\n');
}
