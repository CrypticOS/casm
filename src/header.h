// What to replace '$' with at start of file names
static const char CASM_LOCATION[] = "/home/daniel/Documents/crypticos/";
#define INITIALIZE_ARRAYS 1
#define EMULATOR_WINDOW 0

// Emulator sizes to allocate
#define MAX_INPUT 1000
#define MAX_TOP 100
#define MAX_BOTTOM 10000
#define MAX_LABELS 1000

void run(char *input, char *keys);
void assemble(char *file);