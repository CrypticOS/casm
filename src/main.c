#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

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
			"\tcasm -a <outputfile> -i \"Input\"\tOverride key input with string"
			"Examples:\n" \
			"\tcasm -a foo.casm > a.out && casm -r a.out"
		);
	}

	// Note that we parse backwards.
	char *inputKeys = NULL;
	while (argc != 0) {
		argc--;
		if (argv[argc][0] == '-') {
			switch (argv[argc][1]) {
			case 'i':
				inputKeys = argv[argc + 1];
				break;
			case 'r':
				return run(argv[argc + 1], inputKeys);
			case 'a':
				return assemble(argv[argc + 1], 0);
			}
		}
	}
	
    return 0;
}
