SRC=2048
CC=gcc
CFLAGS=-O3 -Wall -Wextra -I/usr/include/OmegaMKR
LDFLAGS=-lOmegaMKR
OS:=$(shell uname -s)
ifeq ($(OS),Darwin)
	CFLAGS:=$(CFLAGS) -I/Library/Frameworks/SDL2.framework/Versions/A/Headers -I/Library/Frameworks/SDL2_ttf.framework/Versions/A/Headers
	LDFLAGS:=$(LDFLAGS) -framework SDL2 -framework SDL2_ttf
else ifeq ($(OS),Linux)
	CFLAGS:=$(CFLAGS) -I/usr/include/SDL2
	LDFLAGS=-lSDL2 -lSDL2_ttf
endif
BINEXT=out

all: clean build run

clean:
	sudo rm -f $(SRC).$(BINEXT)

build:
	$(CC) $(CFLAGS) $(SRC).c -o $(SRC).$(BINEXT) $(LDFLAGS)

run:
	./$(SRC).$(BINEXT)