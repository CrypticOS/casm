// This is a simple emulator for testing programs
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

#define MAX_INPUT 10

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
		assemble(argv[2]);
	}
	
    return 0;
}
