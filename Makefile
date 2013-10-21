CC=g++
CFLAGS=-Wall -pedantic -g
LIBS=
OBJ=board.o state.o solver.o
APP=mi-ppr-krs

all: ${APP}

${APP}: ${OBJ} main.cpp
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) $(LIBS) -c $<

clean: 
	git clean -fX
