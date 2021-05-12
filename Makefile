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
	cat casm.yaml > ~/.config/micro/syntax/casm.yaml

env:
	echo "export PATH=\$PATH:$PWD" >> ~/.bashrc
