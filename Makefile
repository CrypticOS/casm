INCLUDE ?= $(shell echo ~/Documents/crypticos)/

CC := tcc
CFLAGS := -Wall

all:
	@$(CC) src/*.c $(CFLAGS) -o casm

help:
	@echo "make INCLUDE=~/Documents/crypticos"

window:
	@$(CC) src/*.c $(CFLAGS) -o casm -lX11 -D EMULATOR_WINDOW

micro:
	cp casm.yaml ~/.config/micro/syntax

env:
	echo "export PATH=\$PATH:$PWD" >> ~/.bashrc
