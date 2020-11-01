// This is a simple emulator for testing programs
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT 10000
#define MAX_TOP 10000
#define MAX_BOTTOM 10000
#define MAX_LABELS 10000

#define MAX_TOK 10

enum Types {
	text, digit, string, label,
	var, arr
};

struct Token {
	char text[20];
	int value;
	int length;
	int type;
};

struct Memory {
	struct D {
		char name[20];
		int type;
		int location;
		int length;
	}d[100];
	int length;
	int used;
	int position;
};

// Perform emulator system call
void syscall() {
	
}

// 16 bit emulator
int run(char *input) {
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
        switch (input[c]) {
		case ',':
			if (input[get] == '\0') {
				puts("Read outside input, stopping\n");
				return -1;
			}

			membottom[bottomp] = input[get];
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

	putchar('\n');
}

int isAlpha(char c) {
	if (c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z' || c == '.' || c == '_') {
		return 1;
	} else {
		return 0;
	}
}

int isDigit(char c) {
	if (c >= '0' && c <= '9') {
		return 1;
	} else {
		return 0;
	}
}

// Lex single line, then quit
int lex(struct Token tokens[MAX_TOK], char *line) {
	int c = 0;
	int token = 0;
	while (line[c] != '\0') {
		if (line[c] == '\0' || line[c] == '\n') {
			return token;
		}
		
		// Skip chars
		while (line[c] == ' ' || line[c] == '\t') {
			c++;
		}

		tokens[token].length = 0;
		tokens[token].type = 0;
		tokens[token].value = 0;
		if (isAlpha(line[c])) {
			tokens[token].type = text;
			while (isAlpha(line[c])) {
				tokens[token].text[tokens[token].length] = line[c];
				tokens[token].length++;

				c++;
			}
		} else if (isDigit(line[c])) {
			tokens[token].type = digit;
			while (isDigit(line[c])) {
				tokens[token].value *= 10;
				tokens[token].value += (line[c] - '0');
				c++;
			}
		} else if (line[c] == '\'') {
			tokens[token].type = digit;
			c++;
			tokens[token].value = line[c];
			c += 2; // Skip ', onto next char
		}

		tokens[token].text[tokens[token].length] = '\0';
		token++;
	}

	return token;
}

// Main instruction out function
void out(char *string) {
	printf(string);
}

void putInt(int value) {
	while (value != 0) {
		if (value >= 50) {
			out("%");
			value -= 50;
		} else if (value >= 5) {
			out("*");
			value -= 5;
		} else {
			out("+");
			value--;
		}
	}
}

void assemble() {
	struct Memory memory;
	memory.length = 0;
	memory.used = 0;
	memory.position = 0;
	struct Token tokens[MAX_TOK];
	
	char *text[2] = {
		"var a 9",
		"prt a"
	};

	for (int l = 0; l < 2; l++) {
		int length = lex(tokens, text[l]);
		if (!strcmp(tokens[0].text, "var")) {
			// Add variable into object list
			strcpy(memory.d[memory.length].name, tokens[1].text);
			memory.d[memory.length].length = 1; // vars are 1 int wide
			memory.d[memory.length].location = memory.position;
			memory.length++;

			out("!"); // Reset unitialized value
			putInt(tokens[2].value);
		}
	}
}
int main(int argc, char *argv[]) {
	if (argc != 3) {
		puts("No input.");
		return 0;
	}
	
	if (argv[1][0] == 'r') {
		FILE *reader = fopen(argv[2], "r");
		if (reader == NULL) {
			puts("Bad file.");
			return 0;
		}
		
		char *input = malloc(sizeof(char) * MAX_INPUT);
		fgets(input, MAX_INPUT, reader);
		fclose(reader);
	
		run(input);
		
		free(input);
	} else if (argv[1][0] == 'a') {
		assemble();
	}
	
    return 0;
}
