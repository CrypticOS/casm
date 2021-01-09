cd src
gcc gfx/gfx.c\
	emulate.c\
	lex.c\
	assemble.c\
	main.c\
	-o ../casm -lX11
cd ..
