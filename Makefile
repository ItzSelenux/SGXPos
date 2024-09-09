# Makefile to build a .so file, not tested

CC = cc
CFLAGS = `pkg-config --cflags gtk+-3.0`
LIBS = `pkg-config --libs gtk+-3.0` -lX11
SRC = sgxpos.c
OBJ = $(SRC:.c=.o)
TRG = libsgxpos.so
EXE = sgxpos

all: $(EXE)
sgxpos: sgxpos.o
	$(CC) $(CFLAGS) -o $@ $< $(LIBS) -fPIC -c $(SRC)
	$(CC) -shared -o $(TRG) sgxpos.o
