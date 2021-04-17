#define MAX_TOK 10
#define MAX_LINE 200

// Put in header so lexer can register
// newlines and comments as a line when
// an error is reported
static int line = 0;

enum Types {
	EMPTY, // object is free
	TEXT, DIGIT, STRING, LABEL,
	VAR, ARR,
	RUN, DEFINE, ADDRESSOF,
	WORKSPACE
};

struct Token {
	char text[100];
	int value;
	int length;
	int type;
};

// Multipurpose memory object used for labels, variables,
// defines, arrays.
struct MemObject {
	char name[50];
	int type;
	int location; // NOTE: Also used for value
	int length;
};

// Labels, calls, variables, are all
// stored as memory objects in the same
// structure for simplicity and ease.
struct Memory {
	struct MemObject *d;
	int length; // How many memory objects	
	int used; // How many memory cells used
	int position; // Current bottom position in assembler
};
