tcc emulate.c main.c assemble.c -o casm
./casm a ~/Documents/crypticos/kernel/kernel.casm > foo.o
cat foo.o
echo "" # newline
./casm r foo.o
