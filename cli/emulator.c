// This is a simple emulator for testing programs
#include <stdio.h>
#include <stdlib.h>

#define MAX 10000

int main(int argc, char *argv[]) {
	FILE *reader = fopen(argv[1], "r");
	char input[MAX];
	fgets(input, MAX, reader);
	fclose(reader);

    int memtop[100] = {0};
    int membottom[1000] = {0};

	int topp = 0;
    int bottomp = 0;

    int labels[500];
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
            //printf("%d ", membottom[bottomp]);
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
			//printf("%d %d\n", c, topp);
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

	//putchar('\n');

	//printf("topp = %d", topp);

    return 0;
}