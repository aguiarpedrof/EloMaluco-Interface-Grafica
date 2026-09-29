# Makefile para Elo Maluco 3D - Computacao Grafica (ECOI24 - UNIFEI)
# Compativel com Windows (MinGW) e Linux

CC = gcc
CFLAGS = -Wall -O2 -std=c99
SRC = src/elo_maluco.c src/puzzle.c src/camera.c
TARGET = elo_maluco

ifeq ($(OS),Windows_NT)
    LIBS = -lglut -lopengl32 -lglu32 -lm
    TARGET_BIN = $(TARGET).exe
    RM = del /Q /F
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        LIBS = -lglut -lGLU -lGL -lm
        TARGET_BIN = $(TARGET)
        RM = rm -f
    endif
    ifeq ($(UNAME_S),Darwin)
        LIBS = -framework OpenGL -framework GLUT
        TARGET_BIN = $(TARGET)
        RM = rm -f
    endif
endif

all: $(TARGET_BIN)

$(TARGET_BIN): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET_BIN) $(LIBS)

run: $(TARGET_BIN)
	./$(TARGET_BIN)

clean:
	$(RM) $(TARGET_BIN) *.o
