#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "object.h"
#include "data.h"
#include "header.h"

// Instruction table
enum InstructionEnums {
	I_DEF, I_VAR, I_ARR,
	I_GOT, I_PRT, I_INL,
	I_SUB, I_ADD, I_JMP,
	I_EQU, I_SET, I_RUN,
	I_RET, I_INC, I_FRE
};

static struct Instructions {
	char name[10];
	int id;
}instructions[] = {
	{"def", I_DEF},
	{"var", I_VAR},
	{"arr", I_ARR},
	{"got", I_GOT},
	{"prt", I_PRT},
	{"inl", I_INL},
	{"sub", I_SUB},
	{"add", I_ADD},
	{"jmp", I_JMP},
	{"equ", I_EQU},
	{"set", I_SET},
	{"run", I_RUN},
	{"ret", I_RET},
	{"inc", I_INC},
	{"fre", I_FRE}
};

#define INSTRUCTION_LENGTH sizeof(instructions) / sizeof(instructions[0])

// Recursive file reader variables
char buffer[MAX_LINE];
FILE *readerStack[3];
int readerPoint = 0;

struct Memory memory;

// Main instruction out function
// (send to stdout for now)
void out(char *string) {
	while (*string != '\0') {
		putchar(*string);
		string++;
	}
}

void printError(char error[]) {
	// Only put '\n' if output
	// has been given
	if (line != 0) {
		putchar('\n');
	}
	
	printf("~ERR on line %d: %s~", line + 1, error);
}

// Free entirety of file reader stack
void fileKill() {
	while (readerPoint != 0) {
		fclose(readerStack[readerPoint]);
		readerPoint--;
	}
}

// Check end of current file
int fileNext() {
	fileNext_top:
	if (fgets(buffer, MAX_LINE, readerStack[readerPoint]) == NULL) {
		fclose(readerStack[readerPoint]);
		if (line == 0) {
			printError("Skipping bad file");
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

	// Note: '$' is library location
	if (file[0] == '$') {
		char location[128];
		strcpy(location, CASM_LOCATION);
		strcat(location, file + 1);
		readerStack[readerPoint] = fopen(location, "r");
	} else {
		readerStack[readerPoint] = fopen(file, "r");
	}
	
	if (readerStack[readerPoint] == NULL) {
		printError("Skipping bad file included");
	}
}

void killAll() {
	free(memory.d);
	fileKill();
}

void putInt(int value) {
	// Optimize 48 as
	// !%--, not
	// !*********+++
	while (value != 0) {
		if (value >= 45) {
			out("%");
			value -= 50;
		} else if (value >= 5) {
			out("*");
			value -= 5;
		} else {
			out("+");
			value--;
		}

		while (value < 0) {
			out("-");
			value++;
		}
	}
}

// Locate variable, array, label, from objects in memory from type
int locateObject(char *name, int type) {
	for (int i = 0; i < memory.length; i++) {
		// Make sure to check type first. Name can sometimes be unitialized.
		if (memory.d[i].type == type && !strcmp(memory.d[i].name, name)) {
			return i;
		}
	}

	return -1;
}

// Go to a specific spot in memory
void got(int place) {
	if (place > memory.position) {
		while (place != memory.position) {
			out(">");
			memory.position++;
		}
	} else if (place < memory.position) {
		while (place != memory.position) {
			out("<");
			memory.position--;
		}
	}
}

// Move to location of variable
void gotVar(char *var) {
	int location = locateObject(var, VAR);
	if (location == -1) {
		printError("Variable not found");
		killAll();
		exit(1);
	}
	
	got(memory.d[location].location);
}

// Put/got a token, ready for "^" to be used.
void putTok(struct Token *token, int reset) {
	if (token->type == DIGIT) {
		if (reset) {
			got(memory.used);
		}
		
		out("!");
		putInt(token->value);
	} else if (token->type == TEXT) {
		gotVar(token->text);
	}
}

// Return 0/1 for good/bad assemble
int assemble(char *file, int clean) {
	memory.length = 0;
	memory.used = 0;
	memory.position = 0;

	// Open initial file at zero.
	// (first, before allocations)
	readerStack[0] = fopen(file, "r");
	if (readerStack[0] == NULL) {
		printError("File not found");
		return 1;
	}

	// Use ~32k ram for memory objects
	memory.d = malloc(sizeof(struct MemObject) * MAX_MEMOBJ);
	if (memory.d == NULL) {puts("Alloc error"); return 1;}

	// Default variable WKSP. Takes no space, only
	// set to workspace location.
	strcpy(memory.d[0].name, "WKSP");
	memory.d[0].type = VAR;
	memory.length++;

	for (size_t i = 1; i < MAX_MEMOBJ; i++) {
		memory.d[i].type = EMPTY;
	}
	
	// Lex through the labels/runs first.
	struct Token tokens[MAX_TOK];
	int labelsFound = 0;
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
				int tryDef = locateObject(tokens[i].text, DEFINE);
				if (tryDef != -1) {
					tokens[i].type = DIGIT;
					tokens[i].value = memory.d[tryDef].location;
				}
			} if (tokens[i].type == ADDRESSOF) {
				// For variables get location, for labels get ID
				// (occurence in output file)
				tokens[i].type = DIGIT;
				int location = locateObject(tokens[i].text, VAR);
				if (location == -1) {
					location = locateObject(tokens[i].text, LABEL);
					if (location == -1) {
						printError("Bad request for addressof");
						goto kill;
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

		// Since every instruction starts with TEXT/LABEL,
		// we can assume an error if it is not.
		if (tokens[0].type != TEXT && tokens[0].type != LABEL) {
			printf("%d", tokens[0].type);
			printError("Expected TEXT or LABEL for first token");
			goto kill;
		}

		// If label, it is already added.
		if (tokens[0].type == LABEL) {
			got(memory.used);
			out("|");

			line++;
			continue;
		}

		int inst;
		for (size_t i = 0; i < INSTRUCTION_LENGTH; i++) {
			if (!strcmp(tokens[0].text, instructions[i].name)) {
				inst = instructions[i].id;
				break;
			}
		}

		// Now, assemble actual instructions
		if (!strcmp(tokens[0].text, "def")) {
			strcpy(memory.d[memory.length].name, tokens[1].text);
			memory.d[memory.length].location = tokens[2].value;
			memory.d[memory.length].type = DEFINE;
			memory.length++;
		} else if (inst == I_VAR) {
			// Find a 
			int location = memory.length;
			for (int i = 0; i < memory.length; i++) {
				if (memory.d[i].type == EMPTY) {
					location = i;
					break;
				}
			}
		
			// Add variable into object list
			strcpy(memory.d[location].name, tokens[1].text);
			memory.d[location].location = memory.used;
			memory.d[location].length = 1; // vars are 1 int wide
			memory.d[location].type = VAR;

			// Use if we did not find an available spot
			if (location == memory.length) {
				memory.length++;
			}

			// Allow variables to be unitialized if no value
			// is specified
			if (length != 2) {
				// Go to the variable's spot in memory
				// In order to add value to it.
				got(memory.used);
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
		} else if (inst == I_GOT) {
			if (tokens[1].type == TEXT) {
				gotVar(tokens[1].text);
			} else if (tokens[1].type == DIGIT) {
				got(tokens[1].value);
			}
		} else if (inst == I_PRT) {
			if (tokens[1].type == STRING) {
				got(memory.used);

				// The string printing algorithm is enhanced and optimized.
				for (size_t i = 0; tokens[1].text[i] != '\0'; i++) {
					if (i != 0) {
						if (tokens[1].text[i - 1] < tokens[1].text[i]) {
							putInt(tokens[1].text[i] - tokens[1].text[i - 1]);
							out(".");
							continue;
						} else if (tokens[1].text[i - 1] == tokens[1].text[i]) {
							out(".");
							continue;
						}

						// NOTE: Else go down below
					}

					out("!");
					putInt(tokens[1].text[i]);
					out(".");
				}
			} else {
				putTok(&tokens[1], 1);
				out(".");
			}
		} else if (inst == I_INL) {
			if (tokens[1].type == STRING) {
				out(tokens[1].text);
			} else {
				printError("Expected STRING for INL");
				goto kill;
			}
		} else if (inst == I_SUB) {
			gotVar(tokens[1].text);

			// Since there are no %*+ for subtract, we must
			// do solely -s instead.
			while (tokens[2].value != 0) {
				out("-");
				tokens[2].value--;
			}
			
		} else if (inst == I_ADD) {
			gotVar(tokens[1].text);
			putInt(tokens[2].value);
		} else if (inst == I_JMP) {
			int location = locateObject(tokens[1].text, LABEL);
			if (location == -1) {
				printError("Label not found");
				goto kill;
			}

			got(memory.used);
			out("!"); // Reset current working space cell
			putInt(memory.d[location].location);
			out("^"); // UP
			out("$"); // JMP
		} else if (inst == I_EQU) {
			out("dd"); // Next two are needed as compare values
			putTok(&tokens[1], 1);
			out("^a"); // UP, from second to first compare value
			
			putTok(&tokens[2], 1);
			out("^a"); // UP, from second to first compare value
			
			int location = locateObject( tokens[3].text, LABEL);
			if (location == -1) {
				printError("Label not found");
				goto kill;
			}

			got(memory.used);

			out("!"); // Reset current working space cell
			putInt(memory.d[location].location);
			out("^"); // UP
			out("?"); // EQU
		} else if (inst == I_SET) {
			if (tokens[1].type == TEXT && tokens[2].type == TEXT) {
				gotVar(tokens[2].text);
				out("^");
				gotVar(tokens[1].text);
				out("v");
			} else {
				gotVar(tokens[1].text);
				putTok(&tokens[2], 0);
			}
		} else if (inst == I_RUN) {
			// Find run label
			int i;
			for (i = 0; i < memory.length; i++) {
				// Check type and line in source code.
				if (memory.d[i].type == RUN && memory.d[i].location == line) {
					break;
				}
			}

			got(memory.used); // Go back to original spot
			out("!"); // Reset current working space cell

			// The label number is stored in length
			// (doesn't make sense, but it is fine)
			putInt(memory.d[i].length);
			out("^d"); // UP, Next top cell
			out("!"); // Reset current working space cell

			// Find label like JMP
			int location = locateObject(tokens[1].text, LABEL);
			if (location == -1) {
				printError("Label not found");
				goto kill;
			}

			putInt(memory.d[location].location);
			out("^"); // UP
			out("$"); // JMP
			out("|"); // Put the label for the run command
		} else if (inst == I_RET) {
			got(memory.used); // Go back to original spot
			out("a$"); // BACK, JMP
		} else if (inst == I_INC) {
			fileOpen(tokens[1].text);
			continue;
		} else if (inst == I_FRE) {
			int location = locateObject(tokens[1].text, VAR);
			if (memory.d[location].type != VAR) {
				printError("Can only free variables");
				goto kill;
			}
			
			memory.d[location].type = EMPTY;
			memory.used--;
		}

		line++;
		if (clean) {
			out("\n");
		}
	}

	killAll(&memory);
	return 0;
	
	kill:
	killAll(&memory);
	return 1;
}
