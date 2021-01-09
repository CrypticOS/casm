void run(char *input, char *keys);
void assemble(char *file);

int isAlpha(char c);
int isDigit(char c);
int locateObject(struct Memory *memory, char *name, int type);
int lex(struct Token *tokens, char *line);
