INCLUDE ?= $(shell echo ~/Documents/crypticos)/
FILE := emulate.c \
		lex.c \
		assemble.c \
		main.c

CC := gcc
CFLAGS := -Wall

default: all

help:
	@echo "make INCLUDE=~/Documents/crypticos"

all:
	@cd src; $(CC) $(CFLAGS) \
		$(FILE) \
		-o ../casm

allGfx:
	@cd src; $(CC) $(CFLAGS) \
		$(FILE) \
		-o ../casm -lX11 -D EMULATOR_WINDOW

micro:
	cp casm.yaml ~/.config/micro/syntax

env:
	echo "export PATH=\$PATH:$PWD" >> ~/.bashrc
