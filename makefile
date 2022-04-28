CC = gcc
CFLAGS = -Wall -pedantic -std=gnu99
LDFLAGS = -L/local/courses/csse2310/lib
LDLIBS = -lcsse2310a3
CINCLUDE = -I/local/courses/csse2310/include

PCHFS = sigcat.h.gch hq.h.gch	# PreCompiled Header FileS
OBJS = sigcat.o hq.o		# OBJect fileS
EXECS = sigcat hq		# EXECutable fileS

.PHONY = all sigcat.o ${OBJS} tidy clean
.DEFAULT_GOAL := all

all: clean build

build: sigcat hq tidy

# to make each executable, link object files of the same name
${EXECS}: %: %.o
	@${CC} ${CFLAGS} ${LDFLAGS} ${LDLIBS} ${CINCLUDE} $^ -o $@

# to make each object file, compile source and header files of the same name
${OBJS}: %.o: %.c %.h
	@${CC} ${CFLAGS} ${LDFLAGS} ${LDLIBS} ${CINCLUDE} $^ -c

tidy:
	@rm -rf ${PCHFS} ${OBJS}

clean:
	@rm -f ${EXECS}
