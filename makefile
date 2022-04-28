CC = gcc
CFLAGS = -Wall -pedantic -std=gnu99 -I/local/courses/csse2310/include
LDFLAGS = -L/local/courses/csse2310/lib
LDLIBS = -lcsse2310a3

PCHS = sigcat.h.gch hq.h.gch	# PreCompiled Header files
OBJS = sigcat.o hq.o		# OBJect fileS
EXECS = sigcat hq		# EXECutable fileS

.PHONY = all tidy clean
.DEFAULT_GOAL := all

all: sigcat hq tidy

${EXECS}: %: %.o

${OBJS}: %.o: %.c %.h

tidy:
	@rm -f ${PCHS} ${OBJS}

clean:
	@rm -rf ${PCHS} ${OBJS} ${EXECS} testfiles
