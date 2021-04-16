CC := gcc
CFLAGS := -Wall

default: all

all:
	@cd src; $(CC) $(CFLAGS) gfx/gfx.c \
		emulate.c \
		lex.c \
		assemble.c \
		main.c \
		-o ../casm -lX11

micro:
	cp casm.yaml ~/.config/micro/syntax

env:
	echo "export PATH=\$PATH:$PWD" >> ~/.bashrc
