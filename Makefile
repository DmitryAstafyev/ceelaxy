CC = gcc
CFLAGS = `pkg-config --cflags raylib`
LDFLAGS = `pkg-config --libs raylib`
SYSFLAGS = -lGL -lm -ldl -lpthread -lX11

SRC = \
	src/main.c \
	src/raylib/rlights.c \
	src/units/unit.c \
	src/units/player.c \
	src/models/models.c \
	src/movement/movement.c \
	src/utils/path.c \
	src/game/game.c

TARGET = ceelaxy

all:
	$(CC) -o $(TARGET) $(SRC) $(CFLAGS) $(LDFLAGS) $(SYSFLAGS)

clean:
	rm -f $(TARGET)