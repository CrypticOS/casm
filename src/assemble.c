#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "object.h"
#include "data.h"
#include "header.h"

// For recursive file reader
char buffer[MAX_LINE];
FILE *readerStack[3];
int readerPoint = 0;
int line = 0;

// Free entirety of file reader stack
void fileKill() {
	while (readerPoint != 0) {
		fclose(readerStack[readerPoint]);
		readerPoint--;
	}
}

// Check end of current file
bool fileNext() {
	fileNext_top:
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

			// Recursively call to skip include
			// (We don't want a return value)
			goto fileNext_top;
		}
	}

	return 1;
}

// Pause current file reading, open new one
void fileOpen(char *file) {
	line++; // To skip to line after inc
	readerPoint++;

	// '$' is library location
	if (file[0] == '$') {
		char location[128];
		strcpy(location, CASM_LOCATION);
		strcat(location, file + 1);
		readerStack[readerPoint] = fopen(location, "r");
	} else {
		readerStack[readerPoint] = fopen(file, "r");
	}
	
	if (readerStack[readerPoint] == NULL) {
		puts("ERR: Skipping bad file included");
	}
}

void killAll(struct Memory *memory) {
	free(memory->d);
	fileKill();
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

// Go to a specific spot in memory
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

// Put/got a token, ready for "^" to be used.
void putTok(struct Memory *memory, struct Token *token, bool reset) {
	if (token->type == DIGIT) {
		if (reset) {
			got(memory, memory->used);
		}
		
		out("!");
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

	// Use ~32k ram for memory objects
	memory.d = malloc(sizeof(struct MemObject) * 500);

	// Default variable WKSP. Takes no space, only
	// set to workspace location
	strcpy(memory.d[0].name, "WKSP");
	memory.d[0].type = VAR;
	memory.length++;
	
	struct Token tokens[MAX_TOK];
	int labelsFound = 0;

	readerStack[readerPoint] = fopen(file, "r");
	if (readerStack[readerPoint] == NULL) {
		puts("ERR: File not found.");
		exit(1);
	}
	
	// Lex through the labels/runs first.
	while (fileNext()) {
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
	while (fileNext()) {
		int length = lex(tokens, buffer);
		if (length == 0) {
			continue;
		}

		// Set default variable WKSP before
		// assembling each line
		memory.d[0].location = memory.used;

		// Replace tokens. Replace defined keywords
		// with their values, replace
		for (int i = 0; i < length; i++) {
			if (tokens[i].type == TEXT) {
				int tryDef = locateObject(&memory, tokens[i].text, DEFINE);
				if (tryDef != -1) {
					tokens[i].type = DIGIT;
					tokens[i].value = memory.d[tryDef].location;
				}
			} if (tokens[i].type == ADDRESSOF) {
				// For variables get location, for labels get ID
				// (occurence in output file)
				tokens[i].type = DIGIT;
				int location = locateObject(&memory, tokens[i].text, VAR);
				if (location == -1) {
					location = locateObject(&memory, tokens[i].text, LABEL);
					if (location == -1) {
						puts("ERR: Bad request for addressof.");
						return;
					} else {
						// Label
						tokens[i].value = memory.d[location].location;
					}
				} else {
					// Variable
					// +1 How far back the variable is. Must be after it.
					tokens[i].value = memory.used - memory.d[location].location;
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

			// Allow variables to be unitialized if no value
			// is specified
			if (length != 2) {
				// Go to the variable's spot in memory
				// In order to add value to it.
				got(&memory, memory.used);
				out("!"); // Reset unitialized value
				putInt(tokens[2].value);
			}

			memory.used += 1;
		} else if (!strcmp(tokens[0].text, "arr")) {
			// Add variable into object list
			strcpy(memory.d[memory.length].name, tokens[1].text);
			memory.d[memory.length].location = memory.used;
			memory.d[memory.length].length = tokens[2].value;
			memory.d[memory.length].type = VAR;
			memory.length++;

			// Add up the memory used.
			memory.used += tokens[2].value;

			// Initialize the array if a fourth
			// token is specified. Can be zeros.
			if (length > 3) {
				// If not zero, copy the value instead of making
				// it again every time.
				if (tokens[3].value == 0) {
					while (tokens[2].value != 0) {
						out("!>");
						memory.position++;
						tokens[2].value--;
					}
				} else {
					out("!");
					putInt(tokens[3].value);
					out("^>");
					while (tokens[2].value != 0) {
						out("v>");
						memory.position++;
						tokens[2].value--;
					}
				}
			}
		} else if (!strcmp(tokens[0].text, "got")) {
			if (tokens[1].type == TEXT) {
				gotVar(&memory, tokens[1].text);
			} else if (tokens[1].type == DIGIT) {
				got(&memory, tokens[1].value);
			}
		} else if (!strcmp(tokens[0].text, "str")) {
			for (int c = 0; tokens[1].text[c] != '\0'; c++) {
				memory.position++;
				putInt(tokens[1].text[c]);
				out(">");
			}
		} else if (!strcmp(tokens[0].text, "prt")) {
			if (tokens[1].type == STRING) {
				got(&memory, memory.used);

				// The string printing algorithm is enhanced and optimized.
				for (size_t i = 0; tokens[1].text[i] != '\0'; i++) {
					if (i != 0) {
						if (tokens[1].text[i - 1] < tokens[1].text[i]) {
							putInt(tokens[1].text[i] - tokens[1].text[i - 1]);
							out(".");
						} else if (tokens[1].text[i - 1] == tokens[1].text[i]) {
							out(".");
						}

						continue;
					}

					out("!");
					putInt(tokens[1].text[i]);
					out(".");
				}
			} else {
				putTok(&memory, &tokens[1], 1);
				out(".");
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
				fileKill(&memory);
				exit(0);
			}

			got(&memory, memory.used);
			out("!"); // Reset current working space cell
			putInt(memory.d[location].location);
			out("^"); // UP
			out("$"); // JMP
		} else if (!strcmp(tokens[0].text, "equ")) {
			out("dd"); // Next two are needed as compare values
			putTok(&memory, &tokens[1], 1);
			out("^a"); // UP, from second to first compare value
			
			putTok(&memory, &tokens[2], 1);
			out("^a"); // UP, from second to first compare value
			
			int location = locateObject(&memory, tokens[3].text, LABEL);
			if (location == -1) {
				puts("ERR: Label not found");
				fileKill(&memory);
				exit(0);
			}

			got(&memory, memory.used);

			out("!"); // Reset current working space cell
			putInt(memory.d[location].location);
			out("^"); // UP
			out("?"); // EQU
		} else if (!strcmp(tokens[0].text, "set")) {
			if (tokens[2].type == TEXT) {
				gotVar(&memory, tokens[2].text);
				out("^");
				gotVar(&memory, tokens[1].text);
				out("v");
			} else {
				gotVar(&memory, tokens[1].text);
				putTok(&memory, &tokens[2], 0);
			}
		} else if (!strcmp(tokens[0].text, "run")) {
			// Find run label
			int i = 0;
			while (i < memory.length) {
				// Check type and line in source code.
				if (memory.d[i].type == RUN && memory.d[i].location == line) {
					break;
				}

				i++;
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
				killAll(&memory);
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
		} else if (!strcmp(tokens[0].text, "put")) {
			out("!");
			putInt(tokens[1].value);
		}
		
		line++;
	}

	killAll(&memory);
}
