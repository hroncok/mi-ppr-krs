CC=g++
ifeq "${VERBOSE}" "yes"
	VERBOSE2=-D DEBUG
else
	VERBOSE2=
endif
CFLAGS=-Wall -pedantic -g ${VERBOSE2}
LIBS=
OBJ=board.o state.o solver.o
APP=mi-ppr-krs

all: ${APP}

${APP}: ${OBJ} main.cpp
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

%.o: %.cpp
	$(CC) $(CFLAGS) $(LIBS) -c $<

clean: 
	git clean -fX
