WINDOW ?= 0
INCLUDE ?= $(shell echo ~/Documents/crypticos/)

CC := gcc
CFLAGS := -Wall -D EMULATOR_WINDOW=$(WINDOW) -D CASM_LOCATION='"$(INCLUDE)"'

default: all

help:
	@echo "make WINDOW=0 INCLUDE=~/Documents/crypticos"

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
