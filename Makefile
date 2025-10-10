CC = gcc
CFLAGS = `pkg-config --cflags raylib`
LDFLAGS = `pkg-config --libs raylib`
SYSFLAGS = -lGL -lm -ldl -lpthread -lX11

SRC = \
	src/main.c \
	src/raylib/rlights.c \
	src/units/unit.c \
	src/units/bars.c \
	src/units/player.c \
	src/units/explosion.c \
	src/bullets/bullets.c \
	src/bullets/trail.c \
	src/models/models.c \
	src/textures/textures.c \
	src/sprites/sprites.c \
	src/movement/movement.c \
	src/utils/path.c \
	src/utils/debug.c \
	src/parallax/parallax.c\
	src/game/game.c \
	src/game/levels.c \
	src/game/stat.c

TARGET = ceelaxy

all:
	$(CC) -o $(TARGET) $(SRC) $(CFLAGS) $(LDFLAGS) $(SYSFLAGS)

clean:
	rm -f $(TARGET)