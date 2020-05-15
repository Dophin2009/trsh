CC=gcc
OUT=trsh.out
FILES=trsh.c util.c

default: build

build:
	$(CC) -o $(OUT) $(FILES)

build-debug:
	$(CC) -g -o $(OUT)-debug $(FILES)
