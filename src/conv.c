// Split program size in half, since 16 instructions

#include <stdio.h>
#include <stdlib.h>

char inst[] = "!%*+-<>ad^v.,$?|";

size_t getChar(char a) {
    size_t c;
    for (c = 0; inst[c] != a && inst[c] != '\0'; c++);
    return c;
}

int main(int argc, char *argv[]) {
    for (size_t i = 0; argv[1][i] != '\0'; i++) {
        size_t a = getChar(argv[1][i]);
        i++;
        size_t b = getChar(argv[1][i]);
        printf("%lu, ", (a << 4) | b);
    }

    return 0;
}
