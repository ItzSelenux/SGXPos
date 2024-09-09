CC = cc

CFLAGS = `pkg-config --cflags gtk+-3.0` -I../

LIBS = `pkg-config --libs gtk+-3.0` -lX11
SRC = sgxpos-example.c
OBJ = $(SRC:.c=.o)
EXE = sgxpos-example

PREFIX = /usr
BIN_DIR = $(PREFIX)/bin
APP_DIR = $(PREFIX)/share/applications

all: $(EXE)
sgxpos-example: sgxpos-example.o
	$(CC) $(CFLAGS) -o $@ $< $(LIBS) -lm
debug: sgxpos-example.o
	$(CC) $(CFLAGS) -o $@ $< $(LIBS) -lm -g
test:
	./$(EXE)
clean:
	rm -f $(OBJ) $(EXE) debug
