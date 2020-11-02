#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TOK 10
#define MAX_LINE 200

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
		// Just in case
		if (line[c] == '\n') {
			return token;
		}

		// Skip comments
		if (line[c] == ';') {
			while (line[c] != '\n') {
				c++;
			}
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
			memory->position++;
		}
	} else {
		while (place != memory->position) {
			out("<");
			memory->position--;
		}
	}
}

// Locate var, arr, lbl, from objects in memory
// from type
int locateObject(struct Memory *memory, char *name, int type) {
	for (int i = 0; i < memory->length; i++) {
		if (!strcmp(memory->d[i].name, name) && memory->d[i].type == type) {
			return i;
		}
	}

	return -1;
}

// Not a very useful function, to avoid repeition
void gotVar(struct Memory *memory, char *var) {
	int location = locateObject(memory, var, VAR);
	if (location == -1) {
		puts("ERROR: Variable not found.");
		exit(0);
	}
	
	got(memory, memory->d[location].location);
}

void assemble(char *file) {
	struct Memory memory;
	memory.length = 0;
	memory.used = 0;
	memory.position = 0;
	struct Token tokens[MAX_TOK];

	FILE *reader;
	char buffer[MAX_LINE];
	
	// Lex all labels first.
	int labelsFound = 1;
	reader = fopen(file, "r");
	if (reader == NULL) {
		puts("ERR: Bad file");
		fclose(reader);
		exit(0);
	}

	while (fgets(buffer, MAX_LINE, reader) != NULL) {
		int length = lex(tokens, buffer);
		if (tokens[0].type == LABEL) {
			strcpy(memory.d[memory.length].name, tokens[0].text);
			memory.d[memory.length].location = labelsFound;
			memory.d[memory.length].type = LABEL;
			memory.length++;
		}
	}

	// Close and reopen to pointer
	fclose(reader);
	reader = fopen(file, "r");

	// Lex regular instructions
	while (fgets(buffer, MAX_LINE, reader) != NULL) {
		int length = lex(tokens, buffer);

		// Check instructions
		if (tokens[0].type == LABEL) {
			out("|");
		} else if (!strcmp(tokens[0].text, "var")) {
			// Add variable into object list
			strcpy(memory.d[memory.length].name, tokens[1].text);
			memory.d[memory.length].location = memory.position;
			memory.d[memory.length].length = 1; // vars are 1 int wide
			memory.d[memory.length].type = VAR;
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
		} else if (!strcmp(tokens[0].text, "jmp")) {
			int oldLocation = memory.position;
			got(&memory, memory.used + 1); // Goto working space
			int location = locateObject(&memory, tokens[1].text, LABEL);
			if (location == -1) {
				puts("ERR: Label not found");
				fclose(reader);
				exit(0);
			}

			out("!"); // Reset current working space cell
			putInt(memory.d[location].location);
			out("^"); // UP
			got(&memory, oldLocation); // Go back to original spot
			out("$"); // JMP
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
