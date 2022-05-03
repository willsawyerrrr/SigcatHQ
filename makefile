CC = gcc
CFLAGS = -Wall -pedantic -std=gnu99 -I /local/courses/csse2310/include
LDFLAGS = -L /local/courses/csse2310/lib
LDLIBS = -l csse2310a3

EXECS = sigcat hq				# EXECutable fileS
OBJS = sigcat.o child.o hq.o

.PHONY = all clean
.DEFAULT_GOAL := all

all: ${EXECS}

sigcat: sigcat.o

hq: child.o hq.o

${OBJS}: %.o: %.c %.h

clean:
	@rm -f ${OBJS} ${EXECS} testfiles
