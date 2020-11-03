tcc emulator.c run.c assemble.c -o emulator
./emulator a test.casm > foo.o
./emulator r foo.o
