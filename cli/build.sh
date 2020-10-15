#node node.js "/home/daniel/Documents/os/compile/assembler.casm" > compiler.o
#./emulator compiler.o

# Dummy file
node node.js "/home/daniel/Documents/os/compile/compileNasm.casm" > compiler.o
node node.js "/home/daniel/Documents/os/main.casm" > a.o
./emulator "compiler.o" "`cat a.o`" > nasm.asm
nasm -f elf64 nasm.asm -o nasm.o
ld nasm.o -o nasm
./nasm
