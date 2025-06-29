CC = gcc
CFLAGS = -Wall -Wextra -g `sdl2-config --cflags`
LDFLAGS = `sdl2-config --libs` -lSDL2_image

SRC = src/main.c src/units/unit.c src/units/player.c src/movement/movement.c src/game/game.c
TARGET = galaxy

all:
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

clean:
	rm -f $(TARGET)
