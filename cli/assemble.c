#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TOK 10

enum Types {
	TEXT, DIGIT, STRING, LABEL,
	VAR, ARR
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
			tokens[token].type = TEXT;
			while (isAlpha(line[c])) {
				tokens[token].text[tokens[token].length] = line[c];
				tokens[token].length++;

				c++;
			}

			if (line[c] == ':') {
				tokens[token].type = LABEL;
				c++;
			}
		} else if (isDigit(line[c])) {
			tokens[token].type = DIGIT;
			while (isDigit(line[c])) {
				tokens[token].value *= 10;
				tokens[token].value += (line[c] - '0');
				c++;
			}
		} else if (line[c] == '\'') {
			tokens[token].type = DIGIT;
			c++;
			tokens[token].value = line[c];
			c += 2; // Skip ' and goto next char
		}

		tokens[token].text[tokens[token].length] = '\0';
		token++;
	}

	return token;
}

// Main instruction out function
void out(char *string) {
	while (*string != '\0') {
		putchar(*string);
		string++;
	}
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

// Goto specific spot in memory
void got(struct Memory *memory, int place) {
	if (place > memory->position) {
		while (place != memory->position) {
			out(">");
			place--;
		}
	} else {
		while (place != memory->position) {
			out("<");
			place++;
		}
	}
}

// Locate var, arr, lbl, from objects in memory
int locateObject(struct Memory *memory, char *name) {
	for (int i = 0; i < memory->length; i++) {
		if (!strcmp(memory->d[i].name, name)) {
			return i;
		}
	}

	return -1;
}

// Not a very useful function, to avoid repeition
void gotVar(struct Memory *memory, char *var) {
	int location = locateObject(memory, var);
	if (location == -1) {
		puts("ERROR: Variable not found.");
		exit(0);
	}
	
	got(memory, memory->d[location].location);
}

void assemble() {
	struct Memory memory;
	memory.length = 0;
	memory.used = 0;
	memory.position = 0;
	struct Token tokens[MAX_TOK];
	
	char *text[4] = {
		"var a 3",
		"add a 50",
		"sub a 2",
		"prt a"
	};

	// Lex all labels first.
	int labelsFound = 0;
	for (int l = 0; l < 4; l++) {
		int length = lex(tokens, text[l]);
		if (tokens[0].type == LABEL) {
			strcpy(memory.d[memory.length].name, tokens[0].text);
			memory.d[memory.length].location = labelsFound;
			memory.d[memory.length].type = LABEL;
			memory.length++;
		}
	}

	// Lex regular instructions
	for (int l = 0; l < 4; l++) {
		int length = lex(tokens, text[l]);

		// Check instructions
		if (!strcmp(tokens[0].text, "var")) {
			// Add variable into object list
			strcpy(memory.d[memory.length].name, tokens[1].text);
			memory.d[memory.length].length = 1; // vars are 1 int wide
			memory.d[memory.length].location = memory.position;
			memory.length++;

			out("!"); // Reset unitialized value
			putInt(tokens[2].value);
		} else if (!strcmp(tokens[0].text, "got")) {
			got(&memory, tokens[1].value);
		} else if (!strcmp(tokens[0].text, "prt")) {
			gotVar(&memory, tokens[1].text);
			out(".");
		} else if (!strcmp(tokens[0].text, "add")) {
			gotVar(&memory, tokens[1].text);
			putInt(tokens[2].value);
		} else if (!strcmp(tokens[0].text, "sub")) {
			gotVar(&memory, tokens[1].text);

			// Since there are no %*+ for subtract, we must
			// do solely -s instead.
			while (tokens[2].value != 0) {
				out("-");
				tokens[2].value--;
			}
		}
	}
}
