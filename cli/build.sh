tcc emulate.c assemble/lex.c assemble/assemble.c main.c -o casm
#./casm a ~/Documents/crypticos/kernel/kernel.casm > foo.o
./casm a foo.casm > foo.o
cat foo.o
echo "" # newline
./casm r foo.o
