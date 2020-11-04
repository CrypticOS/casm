tcc casm.c run.c assemble.c -o casm
./casm a test.casm > foo.o
cat foo.o
echo "" # newline
./casm r foo.o
