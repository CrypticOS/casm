tcc emulator.c run.c assemble.c -o emulator
./emulator a test.casm > foo.o
cat foo.o
echo "" # newline
./emulator r foo.o
