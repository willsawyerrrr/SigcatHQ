all: build

build:
	@gcc *.c *.h -pedantic -Wall -std=gnu99 -L/local/courses/csse2310/lib -lcsse2310a3 -I/local/courses/csse2310/include -o sigcat

cleanup:
	@rm sigcat
