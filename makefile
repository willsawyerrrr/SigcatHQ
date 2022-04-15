all: sigcat hq

sigcat:
	@gcc sigcat.c sigcat.h -pedantic -Wall -std=gnu99 -L/local/courses/csse2310/lib -lcsse2310a3 -I/local/courses/csse2310/include -o sigcat

hq:
	@gcc hq.c hq.h -pedantic -Wall -std=gnu99 -L/local/courses/csse2310/lib -lcsse2310a3 -I/local/courses/csse2310/include -o hq

cleanup:
	@rm sigcat hq
