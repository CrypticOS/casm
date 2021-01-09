cd src
tcc -Wall gfx/gfx.c\
	emulate.c\
	lex.c\
	assemble.c\
	main.c\
	-o ../casm -lX11
cd ..
