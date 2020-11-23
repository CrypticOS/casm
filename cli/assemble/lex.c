#include <string.h>
#include <stdbool.h>
#include "object.h"

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

// Locate var, arr, lbl, from objects in memory from type
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

		tokens[token].addressOf = 0;

		if (line[c] == '&') {
			tokens[token].addressOf = 1;
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
