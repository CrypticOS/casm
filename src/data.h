// What to replace '$' with at start of file names
#ifndef CASM_LOCATION
	#define CASM_LOCATION "/home/dan/Documents/crypticos/"
#endif

// Whether to compile with in a X11 windowed
// environment or not.
#ifndef EMULATOR_WINDOW
	#define EMULATOR_WINDOW 0
#endif

// Emulator sizes to allocate
// Will allocate 120kb memory.
#define MAX_TOP 100
#define MAX_BOTTOM 1000
#define MAX_INPUT 100000
#define MAX_LABELS 10000

// Max memory objects (labels, vars, arrs)
#define MAX_MEMOBJ 500

