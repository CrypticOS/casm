// This is a simple emulator for testing programs
#include <stdio.h>
#include <stdlib.h>

#define MAX_INPUT 10000
#define MAX_TOP 10000
#define MAX_BOTTOM 10000
#define MAX_LABELS 10000

int main(int argc, char *argv[]) {
	if (argc != 2) {
		puts("No file.");
		return 0;
	}

	FILE *reader = fopen(argv[1], "r");
	if (reader == NULL) {
		puts("Bad file.");
		return 0;
	}
	
	char *input = malloc(sizeof(char) * MAX_INPUT);
	fgets(input, MAX_INPUT, reader);
	fclose(reader);

	unsigned short *memtop = malloc(sizeof(unsigned short) * MAX_TOP);
    unsigned short *membottom = malloc(sizeof(unsigned short) * MAX_BOTTOM);

	size_t topp = 0;
    size_t bottomp = 0;

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
        switch (input[c]) {
		case ',':
			if (argv[2][get] == '\0') {
				puts("Read outside input, stopping\n");
				return -1;
			}

			membottom[bottomp] = argv[2][get];
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

	free(input);
	free(membottom);
	free(memtop);

	putchar('\n');
	
    return 0;
}
