// This is a simple emulator for testing programs
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

#define MAX_INPUT 1000

int main(int argc, char *argv[]) {
	if (argc != 3) {
		puts(
			"CrypticASM (CASM) Assembler\n" \
			"Usage:\n" \
			"\tcasm a <code.casm>\tAssemble a file\n" \
			"\tcasm r <output.casm\tRun an output file\n" \
			"Examples:\n" \
			"\tcasm a foo.casm > a.out && casm r a.out"
		);
		return 0;
	}
	
	if (argv[1][0] == 'r') {
		FILE *reader = fopen(argv[2], "r");
		if (reader == NULL) {
			puts("Bad file.");
			fclose(reader);
			return 0;
		}
		
		char *input = malloc(sizeof(char) * MAX_INPUT);
		fgets(input, MAX_INPUT, reader);
		fclose(reader);
	
		run(input);
		
		free(input);
	} else if (argv[1][0] == 'a') {
		assemble(argv[3]);
	}
	
    return 0;
}
