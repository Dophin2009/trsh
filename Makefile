CC=gcc
OUT=trsh
FILES=trsh.c builtins.c env.c util.c

default: build

build:
	$(CC) -o $(OUT).out $(FILES)

build-debug:
	$(CC) -g -o $(OUT)-debug.out $(FILES)
