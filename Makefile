CC=mpic++
ifeq "${VERBOSE}" "yes"
	VERBOSE2=-D DEBUG
else
	VERBOSE2=
endif
CFLAGS=-Wall -Wno-long-long -pedantic -g ${VERBOSE2}
LIBS=
OBJ=board.o state.o solver.o logger.o
APP=mi-ppr-krs

all: ${APP}

${APP}: ${OBJ} main.cpp
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

%.o: %.cpp
	$(CC) $(CFLAGS) $(LIBS) -c $<

clean: 
	git clean -fX
