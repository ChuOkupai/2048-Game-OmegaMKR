OS_NAME := $(shell uname)
ifeq ($(OS_NAME), Darwin)
		SDL_INC := -I"/Library/Frameworks/SDL2.framework/Versions/A/Headers" -I"/Library/Frameworks/SDL2_ttf.framework/Versions/A/Headers"
		SDL_LIB := -framework SDL2 -framework SDL2_ttf
else
		SDL_INC := -I"/usr/include/SDL2"
		SDL_LIB := -lSDL2 -lSDL2_ttf
endif
GAME_NAME := 2048.out

all: clean build run

clean:
	sudo rm -f $(GAME_NAME)
	
build:
	gcc -Wall -I"/usr/include/OmegaMKR" $(SDL_INC) 2048.c -o $(GAME_NAME) -lOmegaMKR $(SDL_LIB)

run:
	./2048.out
