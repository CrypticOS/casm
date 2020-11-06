tcc casm.c run.c assemble.c -o casm
./casm a foo.casm > foo.o
cat foo.o
echo "" # newline
./casm r foo.o
