# Dummy file
node node.js "/home/daniel/Documents/os/casm/compiler.casm" > compiler.o
./emulator "compiler.o" "!%***.!**.0" > nasm.asm
nasm -f elf64 nasm.asm -o nasm.o
ld nasm.o -o nasm
./nasm
