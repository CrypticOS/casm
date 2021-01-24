// What to replace '$' with at start of file names
static const char CASM_LOCATION[] = "/home/dan/Documents/crypticos/";
#define INITIALIZE_ARRAYS 1

// Whether to compile with in a X11 windowed
// emulator or not.
#define EMULATOR_WINDOW 0

// Emulator sizes to allocate
// Will allocate 120kb memory.
#define MAX_TOP 100
#define MAX_BOTTOM 1000
#define MAX_INPUT 100000
#define MAX_LABELS 10000
#define MAX_MEMOBJ 500

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
}instructions[15] = {
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
