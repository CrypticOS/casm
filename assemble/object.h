#define MAX_TOK 10
#define MAX_LINE 200

enum Types {
	TEXT, DIGIT, STRING, LABEL,
	VAR, ARR,
	RUN, DEFINE,
	WORKSPACE
};

struct Token {
	char text[100];
	int value;
	int length;
	int type;
	bool addressOf;
};

// memory object for vars, arrs, labels, etc
struct MemObject {
	char name[50];
	int type;
	int location;
	int length;
};

// Labels, calls, variables, are all
// stored as memory objects in the same
// structure.
struct Memory {
	struct MemObject *d;
	int length;
	int used;
	int position;
};