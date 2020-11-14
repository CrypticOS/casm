#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_TOK 10
#define MAX_LINE 200

enum Types {
	TEXT, DIGIT, STRING, LABEL,
	VAR, ARR,
	RUN, DEFINE,
	WORKSPACE
};

struct Token {
	char text[50];
	int value;
	int length;
	int type;
};

// Labels, calls, variables, are all
// stored as memory objects in the same
// structure.
struct Memory {
	struct D {
		char name[50];
		int type;
		int location;
		int length;
	}d[200];
	int length;
	int used;
	int position;
};

// For recursive file reader
char buffer[MAX_LINE];
FILE *readerStack[3];
int readerPoint = 0;
int line = 0;

// free all file readers
void fileKill() {
	while (readerPoint != 0) {
		fclose(readerStack[readerPoint]);
		readerPoint--;
	}
}

// Check end of current file
bool fileNext() {
	if (fgets(buffer, MAX_LINE, readerStack[readerPoint]) == NULL) {
		fclose(readerStack[readerPoint]);
		if (line == 0) {
			puts("ERR: Skipping bad file");
			readerPoint--;
		}

		if (readerPoint == 0) {
			return 0; // End of read
		} else {
			readerPoint--;
			fileNext(); // Recursively call to skip include
		}
	}

	return 1;
}

// Pause current file reading, open new one
void fileOpen(char *file) {
	line++; // To skip to line after inc
	readerPoint++;
	readerStack[readerPoint] = fopen(file, "r");
	if (readerStack[readerPoint] == NULL) {
		puts("ERR: Skipping bad file included");
	}
}


int isAlpha(char c) {
	if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c == '.' || c == '_')) {
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

// Locate var, arr, lbl, from objects in memory
// from type
int locateObject(struct Memory *memory, char *name, int type) {
	for (int i = 0; i < memory->length; i++) {
		// Make sure to check type first. Name can sometimes be unitialized.
		if (memory->d[i].type == type && !strcmp(memory->d[i].name, name)) {
			return i;
		}
	}

	return -1;
}

// Lex single line, then quit
int lex(struct Token *tokens, char *line) {
	int c = 0;
	int token = 0;
	while (line[c] != '\0') {
		// Skip chars
		while (line[c] == ' ' || line[c] == '\t') {
			c++;
		}

		// Skip comments
		if (line[c] == ';') {
			while (line[c] != '\n' || line[c] == '\0') {
				c++;
			}
		}

		// Check if this is a nothing line (comments, blank)
		if (line[c] == '\n' || line[c] == '\0') {
			return token;
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
		} else if (line[c] == '"') {
			tokens[token].type = STRING;
			c++; // Skip "
			while (line[c] != '"') {
				tokens[token].text[tokens[token].length] = line[c];
				tokens[token].length++;
				c++;
			}

			c++; // Skip "
		}

		// Always null terminate string
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
	} else if (place < memory->position) {
		while (place != memory->position) {
			out("<");
			memory->position--;
		}
	}
}

// Goto a variable location.
void gotVar(struct Memory *memory, char *var) {
	int location = locateObject(memory, var, VAR);
	if (location == -1) {
		puts("ERROR: Variable not found.");
		exit(0);
	}
	
	got(memory, memory->d[location].location);
}

// Put/got int or var
void putVal(struct Memory *memory, struct Token *token) {
	if (token->type == DIGIT) {
		got(memory, memory->used);
		out("!"); // Reset working space
		putInt(token->value);
	} else if (token->type == TEXT) {
		gotVar(memory, token->text);
	}
}

void assemble(char *file) {
	struct Memory memory;
	memory.length = 0;
	memory.used = 0;
	memory.position = 0;
	struct Token tokens[MAX_TOK];

	int labelsFound = 1;
	bool run = 1; // For recursive while loop

	readerStack[readerPoint] = fopen(file, "r");
	
	// Lex through the labels/runs first.
	while (1) {
		run = fileNext();
		if (!run) {
			break;
		}
		
		int length = lex(tokens, buffer);
		if (length == 0) {
			continue;
		}
		
		if (tokens[0].type == LABEL) {
			strcpy(memory.d[memory.length].name, tokens[0].text);
			memory.d[memory.length].location = labelsFound;
			memory.d[memory.length].type = LABEL;
			memory.length++;
			labelsFound++;
		} else if (tokens[0].type == TEXT && !strcmp(tokens[0].text, "run")) {
			memory.d[memory.length].location = line;
			memory.d[memory.length].length = labelsFound; // Remember labelsFound is stored in length.
			memory.d[memory.length].type = RUN;
			memory.length++;
			labelsFound++;
		} else if (!strcmp(tokens[0].text, "inc")) {
			fileOpen(tokens[1].text);
			continue;
		}
		
		line++;
	}

	// Close and reopen to pointer
	readerPoint = 0;
	readerStack[readerPoint] = fopen(file, "r");

	// Lex regular instructions
	line = 0;
	while (1) {
		run = fileNext();
		if (!run) {
			break;
		}
		
		int length = lex(tokens, buffer);
		if (length == 0) {
			continue;
		}

		// Try to match define with tokens
		for (int i = 0; i < length; i++) {
			if (tokens[i].type == TEXT) {
				int tryDef = locateObject(&memory, tokens[i].text, DEFINE);
				if (tryDef != -1) {
					tokens[i].type = DIGIT;
					tokens[i].value = memory.d[tryDef].location;
				}
			}
		}

		// Instruction Assembler
		if (!strcmp(tokens[0].text, "def")) {
			strcpy(memory.d[memory.length].name, tokens[1].text);
			memory.d[memory.length].location = tokens[2].value;
			memory.d[memory.length].type = DEFINE;
			memory.length++;
		} else if (tokens[0].type == LABEL) {
			got(&memory, memory.used);
			out("|");
		} else if (!strcmp(tokens[0].text, "var")) {
			// Add variable into object list
			strcpy(memory.d[memory.length].name, tokens[1].text);
			memory.d[memory.length].location = memory.used;
			memory.d[memory.length].length = 1; // vars are 1 int wide
			memory.d[memory.length].type = VAR;
			memory.length++;

			// Go to the variable's spot in memory
			// In order to add value to it.
			got(&memory, memory.used);
			memory.used += 1;
			out("!"); // Reset unitialized value
			putInt(tokens[2].value);
			// out(">");
			// memory.used++;
			// memory.position++;
		} else if (!strcmp(tokens[0].text, "arr")) {
			// Add variable into object list
			strcpy(memory.d[memory.length].name, tokens[1].text);
			memory.d[memory.length].location = memory.used;
			memory.d[memory.length].length = tokens[2].value; // vars are 1 int wide
			memory.d[memory.length].type = VAR;
			memory.length++;

			// Go to the variable's spot in memory
			// In order to add value to it.
			memory.used += tokens[2].value;
		} else if (!strcmp(tokens[0].text, "got")) {
			if (tokens[1].type == TEXT) {
				if (!strcmp(tokens[1].text, "WKSP")) {
					got(&memory, memory.used);
				} else {
					gotVar(&memory, tokens[1].text);
				}
			} else if (tokens[1].type == DIGIT) {
				got(&memory, tokens[1].value);
			}
		} else if (!strcmp(tokens[0].text, "str")) {
			for (int c = 0; tokens[1].text[c] != '\0'; c++) {
				memory.position++;
				out(">");
			}
		} else if (!strcmp(tokens[0].text, "prt")) {
			if (tokens[1].type == DIGIT) {
				got(&memory, memory.used);
				out("!");
				putInt(tokens[1].value);
				out(".");
			} else if (tokens[1].type == TEXT) {
				gotVar(&memory, tokens[1].text);
				out(".");
			} else if (tokens[1].type == STRING) {
				for (int i = 0; tokens[1].text[i] != '\0'; i++) {
					if (i != 0) {
						if (tokens[1].text[i - 1] < tokens[i].text[i]) {
							putInt(tokens[1].text[i] - tokens[1].text[i - 1]);
							out(".");
							continue;
						} else if (tokens[1].text[i - 1] == tokens[1].text[i]) {
							out(".");
							continue;
						}
					}

					out("!");
					putInt(tokens[1].text[i]);
					out(".");
				}
			}
		} else if (!strcmp(tokens[0].text, "inl")) {
			out(tokens[1].text);
		} else if (!strcmp(tokens[0].text, "sub")) {
			gotVar(&memory, tokens[1].text);

			// Since there are no %*+ for subtract, we must
			// do solely -s instead.
			while (tokens[2].value != 0) {
				out("-");
				tokens[2].value--;
			}
			
		} else if (!strcmp(tokens[0].text, "add")) {
			gotVar(&memory, tokens[1].text);
			putInt(tokens[2].value);
		} else if (!strcmp(tokens[0].text, "jmp")) {
			int location = locateObject(&memory, tokens[1].text, LABEL);
			if (location == -1) {
				puts("ERR: Label not found");
				fileKill();
				exit(0);
			}

			got(&memory, memory.used);
			out("!"); // Reset current working space cell
			putInt(memory.d[location].location);
			out("^"); // UP
			out("$"); // JMP
		} else if (!strcmp(tokens[0].text, "equ")) {
			out("dd"); // Next two are needed as compare values
			putVal(&memory, &tokens[1]);
			out("^a"); // UP, from second to first compare value
			
			putVal(&memory, &tokens[2]);
			out("^a"); // UP, from second to first compare value
			
			int location = locateObject(&memory, tokens[3].text, LABEL);
			if (location == -1) {
				puts("ERR: Label not found");
				fileKill();
				exit(0);
			}

			got(&memory, memory.used);

			out("!"); // Reset current working space cell
			putInt(memory.d[location].location);
			out("^"); // UP
			out("?"); // EQU
		} else if (!strcmp(tokens[0].text, "set")) {
			//int oldLocation = memory.position;
			if (tokens[2].type == DIGIT) {
				gotVar(&memory, tokens[1].text);
				out("!");
				putInt(tokens[2].value);
			} else if (tokens[2].type == TEXT) {
				gotVar(&memory, tokens[2].text);
				out("^");
				gotVar(&memory, tokens[1].text);
				out("v");
			}

			//got(&memory, oldLocation);
			//got(&memory, memory.used);
		} else if (!strcmp(tokens[0].text, "run")) {
			// Find run label
			int i = 0;
			for (; i < memory.length; i++) {
				// Check type and line in source code.
				if (memory.d[i].type == RUN && memory.d[i].location == line) {
					break;
				}
			}

			got(&memory, memory.used); // Go back to original spot
			
			out("!"); // Reset current working space cell

			// The label number is stored in length
			// (doesn't make sense, but it is fine)
			putInt(memory.d[i].length);
			out("^d"); // UP, Next top cell

			out("!"); // Reset current working space cell

			// Find label like JMP
			int location = locateObject(&memory, tokens[1].text, LABEL);
			if (location == -1) {
				puts("ERR: Label not found");
				fileKill();
				exit(0);
			}

			putInt(memory.d[location].location);

			out("^"); // UP
			out("$"); // JMP

			out("|"); // Put the label for the run command
		} else if (!strcmp(tokens[0].text, "ret")) {
			got(&memory, memory.used); // Go back to original spot
			out("a$"); // BACK, JMP
		} else if (!strcmp(tokens[0].text, "inc")) {
			fileOpen(tokens[1].text);
			continue;
		}

		line++;
	}

	fileKill();
}
