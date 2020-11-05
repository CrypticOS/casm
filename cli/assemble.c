#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TOK 10
#define MAX_LINE 200

enum Types {
	TEXT, DIGIT, STRING, LABEL,
	VAR, ARR,
	RUN,
	WORKSPACE
};

struct Token {
	char text[50];
	int value;
	int length;
	int type;
};

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
		// Skip comments
		if (line[c] == ';') {
			while (line[c] != '\n' || line[c] == '\0') {
				c++;
			}
		}
		
		// Skip chars
		while (line[c] == ' ' || line[c] == '\t') {
			c++;
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
			tokens[token].type == STRING;
			c++; // Skip "
			while (line[c] != '"') {
				tokens[token].text[tokens[token].length] = line[c];
				tokens[token].length++;
			}

			c++; // Skip "
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
	} else if (place < memory->position) {
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
		// Make sure to check type first. Name can sometimes be unitialized.
		if (memory->d[i].type == type && !strcmp(memory->d[i].name, name)) {
			return i;
		}
	}

	return -1;
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
		got(memory, memory->used); // Goto working space
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

	FILE *reader;
	char buffer[MAX_LINE];
	
	// Lex all labels first.
	int labelsFound = 1;
	int line = 0;
	reader = fopen(file, "r");
	if (reader == NULL) {
		puts("ERR: Bad file");
		fclose(reader);
		exit(0);
	}

	while (fgets(buffer, MAX_LINE, reader) != NULL) {
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
		}

		line++;
	}

	// Close and reopen to pointer
	fclose(reader);
	reader = fopen(file, "r");

	// Lex regular instructions
	line = 0;
	while (fgets(buffer, MAX_LINE, reader) != NULL) {
		int length = lex(tokens, buffer);
		if (length == 0) {
			continue;
		}

		// Instruction Assembler
		if (tokens[0].type == LABEL) {
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
			out("!"); // Reset unitialized value
			putInt(tokens[2].value);
			out(">");
			memory.used++;
			memory.position++;
		} else if (!strcmp(tokens[0].text, "got")) {
			if (tokens[1].type == TEXT && !strcmp(tokens[1].text, "WKSP")) {
				
			} else if (tokens[1].type == DIGIT) {
				got(&memory, tokens[1].value);
			}
		} else if (!strcmp(tokens[0].text, "prt")) {
			if (tokens[1].type == DIGIT) {
				out("!");
				putInt(tokens[1].value);
			} else if (tokens[1].type == TEXT) {
				gotVar(&memory, tokens[1].text);
			}

			out(".");

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
				fclose(reader);
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
				fclose(reader);
				exit(0);
			}

			out("!"); // Reset current working space cell
			putInt(memory.d[location].location);
			out("^"); // UP
			got(&memory, memory.used);
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
				fclose(reader);
				exit(0);
			}

			putInt(memory.d[location].location);

			out("^"); // UP
			got(&memory, memory.used); // Go back to original spot
			out("$"); // JMP

			out("|"); // Put the label for the run command
		} else if (!strcmp(tokens[0].text, "ret")) {
			got(&memory, memory.used); // Go back to original spot
			out("a$"); // BACK, JMP
		}

		//putchar(' ');
		line++;
	}

	fclose(reader);
}
