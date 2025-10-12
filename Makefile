# Compiler and flags
CC       := gcc
CSTD     := -std=c11
WARN     := -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Wpointer-arith
OPT      := -O2 -g
CFLAGS   := $(CSTD) $(WARN) $(OPT) `pkg-config --cflags raylib`
LDFLAGS  := `pkg-config --libs raylib`

SYSFLAGS := -lm -ldl -lpthread

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    SYSFLAGS :=
endif

TARGET := ceelaxy

SRC := \
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
    src/utils/resolution.c \
    src/parallax/parallax.c \
    src/game/game.c \
    src/game/levels.c \
    src/game/stat.c

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) $(SYSFLAGS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
