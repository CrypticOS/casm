#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
int findObject(struct Memory *memory, char *name) {
	for (int i = 0; i < memory->length; i++) {
		if (!strcmp(memory->d[i].name, name)) {
			return i;
		}
	}

	return -1;
}

void assemble() {
	struct Memory memory;
	memory.length = 0;
	memory.used = 0;
	memory.position = 0;
	struct Token tokens[MAX_TOK];
	
	char *text[2] = {
		"var a 9",
		"got 1"
	};

	for (int l = 0; l < 2; l++) {
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
		}
	}
}
