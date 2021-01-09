#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data.h"
#include "object.h"
#include "header.h"

int main(int argc, char *argv[]) {
	if (argc == 1) {
		puts(
			"CrypticASM (CASM) Assembler\n" \
			"Usage:\n" \
			"\tcasm -a <code.casm>\tAssemble a file\n" \
			"\tcasm -r <outputfile>\tRun an output file\n" \
			"Examples:\n" \
			"\tcasm -a foo.casm > a.out && casm -r a.out"
		);
	}
	
	char *inputKeys = NULL;
	while (argc != 0) {
		argc--;
		if (argv[argc][0] == '-') {
			switch (argv[argc][1]) {
			case 'i':
				inputKeys = argv[argc + 1];
				break;
			case 'r':
				{
					FILE *reader = fopen(argv[argc + 1], "r");
					if (reader == NULL) {
						puts("ERR: File not found.");
						return 1;
					}
			
					// Copy file into memory
					char *input = malloc(sizeof(char) * MAX_INPUT);
					char *index = input;
					while (1) {
						int c = fgetc(reader);
						if (feof(reader)) {
							break;
						}
			
						*index = (char)c;
						index++;
					}
			
					*index = '\0';
					
					fclose(reader);
					run(input, inputKeys);
					free(input);
			
					putchar('\n');
					break;
				}
			case 'a':
				assemble(argv[argc + 1]);
				break;
			}
		}
	}
	
    return 0;
}
